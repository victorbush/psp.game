/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "thirdparty/cimgui/imgui_jetz.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

#include "autogen/vlk_imgui_pipeline.static.h"

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_imgui_pipeline__construct
	(
	_vlk_imgui_pipeline_t*			pipeline,
	_vlk_dev_t*						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	)
{
	clear_struct(pipeline);
	pipeline->dev = device;
	pipeline->extent = extent;
	pipeline->render_pass = render_pass;

	create_buffers(pipeline);
	create_font_texture(pipeline);
	create_descriptor_pool(pipeline);
	create_descriptor_layout(pipeline);
	create_descriptor_sets(pipeline);
	create_pipeline_layout(pipeline);
	create_pipeline(pipeline);
}

void _vlk_imgui_pipeline__destruct(_vlk_imgui_pipeline_t* pipeline)
{
	destroy_pipeline(pipeline);
	destroy_pipeline_layout(pipeline);
	destroy_descriptor_sets(pipeline);
	destroy_descriptor_layout(pipeline);
	destroy_descriptor_pool(pipeline);
	destroy_font_texture(pipeline);
	destroy_buffers(pipeline);
}

void _vlk_imgui_pipeline__bind(_vlk_imgui_pipeline_t* pipeline, VkCommandBuffer cmd)
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle);
}

void _vlk_imgui_pipeline__render(_vlk_imgui_pipeline_t* pipeline, _vlk_frame_t* frame, ImDrawData* draw_data)
{
	if (!draw_data)
		return;

	/*
	Update vertex and index buffers
	*/
	uint32_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
	uint32_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);

	if (vertex_size == 0 || index_size == 0)
		return;

	if (!pipeline->vertex_buffer_is_init.data[frame->image_idx]
		|| pipeline->vertex_buffer_sizes.data[frame->image_idx] < vertex_size)
	{
		if (pipeline->vertex_buffer_is_init.data[frame->image_idx])
		{
			_vlk_buffer__destruct(&pipeline->vertex_buffers.data[frame->image_idx]);
		}

		_vlk_buffer__construct(&pipeline->vertex_buffers.data[frame->image_idx], pipeline->dev, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		pipeline->vertex_buffer_sizes.data[frame->image_idx] = vertex_size;
		pipeline->vertex_buffer_is_init.data[frame->image_idx] = TRUE;
	}

	if (!pipeline->index_buffer_is_init.data[frame->image_idx]
		|| pipeline->index_buffer_sizes.data[frame->image_idx] < index_size)
	{
		if (pipeline->index_buffer_is_init.data[frame->image_idx])
		{
			_vlk_buffer__destruct(&pipeline->index_buffers.data[frame->image_idx]);
		}

		_vlk_buffer__construct(&pipeline->index_buffers.data[frame->image_idx], pipeline->dev, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		pipeline->index_buffer_sizes.data[frame->image_idx] = index_size;
		pipeline->index_buffer_is_init.data[frame->image_idx] = TRUE;
	}

	// Upload Vertex and index Data:
	{
		ImDrawVert* vertex_dest = NULL;
		ImDrawIdx* index_dest = NULL;

		VmaAllocation vertex_alloc = pipeline->vertex_buffers.data[frame->image_idx].allocation;
		VmaAllocation index_alloc = pipeline->index_buffers.data[frame->image_idx].allocation;

		VkResult result = vmaMapMemory(pipeline->dev->allocator, vertex_alloc, (void**)&vertex_dest);
		if (result != VK_SUCCESS)
		{
			log__fatal("Error");
		}

		result = vmaMapMemory(pipeline->dev->allocator, index_alloc, (void**)&index_dest);
		if (result != VK_SUCCESS)
		{
			log__fatal("Error");
		}

		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			memcpy(vertex_dest, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(index_dest, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vertex_dest += cmd_list->VtxBuffer.Size;
			index_dest += cmd_list->IdxBuffer.Size;
		}

		vmaUnmapMemory(pipeline->dev->allocator, vertex_alloc);
		vmaUnmapMemory(pipeline->dev->allocator, index_alloc);
	}

	/*
	Bind pipeline and descriptor sets
	*/
	vkCmdBindPipeline(frame->cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle);
	vkCmdBindDescriptorSets(frame->cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->layout, 0, 1, &pipeline->descriptor_sets[frame->image_idx], 0, NULL);

	/*
	Bind buffers
	*/
	VkBuffer vertex_buffers[1] = { pipeline->vertex_buffers.data[frame->image_idx].handle };
	VkDeviceSize vertex_offset[1] = { 0 };
	vkCmdBindVertexBuffers(frame->cmd_buf, 0, 1, vertex_buffers, vertex_offset);
	vkCmdBindIndexBuffer(frame->cmd_buf, pipeline->index_buffers.data[frame->image_idx].handle, 0, VK_INDEX_TYPE_UINT16);

	/*
	Setup viewport
	*/
	draw_data->DisplaySize.x = (float)pipeline->extent.width;
	draw_data->DisplaySize.y = (float)pipeline->extent.height;

	VkViewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = draw_data->DisplaySize.x;
	viewport.height = draw_data->DisplaySize.y;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(frame->cmd_buf, 0, 1, &viewport);

	// Setup scale and translation:
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
	{
		float scale[2];
		scale[0] = 2.0f / draw_data->DisplaySize.x;
		scale[1] = 2.0f / draw_data->DisplaySize.y;
		float translate[2];
		translate[0] = -1.0f - draw_data->DisplayPos.x * scale[0];
		translate[1] = -1.0f - draw_data->DisplayPos.y * scale[1];
		vkCmdPushConstants(frame->cmd_buf, pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
		vkCmdPushConstants(frame->cmd_buf, pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);
	}

	// Render the command lists:
	int vtx_offset = 0;
	int idx_offset = 0;
	ImVec2 display_pos = draw_data->DisplayPos;

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer.Data[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Apply scissor/clipping rectangle
				// FIXME: We could clamp width/height based on clamped min/max values.
				VkRect2D scissor;
				scissor.offset.x = (int32_t)(pcmd->ClipRect.x - display_pos.x) > 0 ? (int32_t)(pcmd->ClipRect.x - display_pos.x) : 0;
				scissor.offset.y = (int32_t)(pcmd->ClipRect.y - display_pos.y) > 0 ? (int32_t)(pcmd->ClipRect.y - display_pos.y) : 0;
				scissor.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
				scissor.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y + 1); // FIXME: Why +1 here?
				vkCmdSetScissor(frame->cmd_buf, 0, 1, &scissor);

				// Draw
				vkCmdDrawIndexed(frame->cmd_buf, pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## static
/**
Creates vertex and index buffer arrays.
*/
static void create_buffers(_vlk_imgui_pipeline_t* pipeline)
//##
{
	/*
	Resize buffer arrays. Buffers are created and updated as needed at render time.
	*/
	utl_array_init(&pipeline->index_buffers);
	utl_array_init(&pipeline->index_buffer_sizes);
	utl_array_init(&pipeline->index_buffer_is_init);
	utl_array_init(&pipeline->vertex_buffers);
	utl_array_init(&pipeline->vertex_buffer_sizes);
	utl_array_init(&pipeline->vertex_buffer_is_init);

	utl_array_resize(&pipeline->index_buffers, NUM_FRAMES);
	utl_array_resize(&pipeline->index_buffer_sizes, NUM_FRAMES);
	utl_array_resize(&pipeline->index_buffer_is_init, NUM_FRAMES);
	utl_array_resize(&pipeline->vertex_buffers, NUM_FRAMES);
	utl_array_resize(&pipeline->vertex_buffer_sizes, NUM_FRAMES);
	utl_array_resize(&pipeline->vertex_buffer_is_init, NUM_FRAMES);

	for (uint32_t i = 0; i < pipeline->vertex_buffers.count; ++i)
	{
		pipeline->index_buffer_sizes.data[i] = 0;
		pipeline->vertex_buffer_sizes.data[i] = 0;

		pipeline->index_buffer_is_init.data[i] = FALSE;
		pipeline->vertex_buffer_is_init.data[i] = FALSE;
	}
}

//## static
/**
Creates the descriptor set layout.
*/
static void create_descriptor_layout(_vlk_imgui_pipeline_t* pipeline)
//##
{
	/* Font texture */
	VkDescriptorSetLayoutBinding font_texture_binding;
	clear_struct(&font_texture_binding);
	font_texture_binding.binding = 0;
	font_texture_binding.descriptorCount = 1;
	font_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	font_texture_binding.pImmutableSamplers = NULL;
	font_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding bindings[] =
	{
		font_texture_binding
	};

	VkDescriptorSetLayoutCreateInfo layout_info;
	clear_struct(&layout_info);
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = cnt_of_array(bindings);
	layout_info.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(pipeline->dev->handle, &layout_info, NULL, &pipeline->descriptor_layout) != VK_SUCCESS)
	{
		log__fatal("Failed to create descriptor set layout.");
	}
}

//## static
/**
Creates a descriptor pool for this layout. Descriptor sets are allocated
from the pool.
*/
static void create_descriptor_pool(_vlk_imgui_pipeline_t* pipeline)
//##
{
	// TODO ???

	VkDescriptorPoolSize pool_sizes[1];
	memset(pool_sizes, 0, sizeof(pool_sizes));
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[0].descriptorCount = NUM_FRAMES;

	VkDescriptorPoolCreateInfo pool_info;
	clear_struct(&pool_info);
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = cnt_of_array(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	// TOOD : what should maxSets be??
	pool_info.maxSets = NUM_FRAMES;

	if (vkCreateDescriptorPool(pipeline->dev->handle, &pool_info, NULL, &pipeline->descriptor_pool) != VK_SUCCESS) 
	{
		log__fatal("Failed to create descriptor pool.");
	}
}

//## static
/**
Allocates a descriptor set for each possible concurrent frame.
*/
static void create_descriptor_sets(_vlk_imgui_pipeline_t* pipeline)
//##
{
	VkDescriptorSetLayout layouts[NUM_FRAMES];
	layouts[0] = pipeline->descriptor_layout;
	layouts[1] = pipeline->descriptor_layout;

	VkDescriptorSetAllocateInfo alloc_info;
	clear_struct(&alloc_info);
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = pipeline->descriptor_pool;
	alloc_info.descriptorSetCount = cnt_of_array(pipeline->descriptor_sets);
	alloc_info.pSetLayouts = layouts;

	VkResult result = vkAllocateDescriptorSets(pipeline->dev->handle, &alloc_info, pipeline->descriptor_sets);
	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to allocate descriptor sets.");
	}

	for (int i = 0; i < NUM_FRAMES; ++i)
	{
		VkWriteDescriptorSet descriptor_writes[1];
		size_t write_idx = 0;

		/* Font texture */
		VkDescriptorImageInfo image_info;
		clear_struct(&image_info);
		image_info.imageLayout = pipeline->font_texture.image_info.imageLayout;
		image_info.imageView = pipeline->font_texture.image_info.imageView;
		image_info.sampler = pipeline->font_texture_sampler;

		clear_struct(&descriptor_writes[0]);
		descriptor_writes[write_idx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[write_idx].dstSet = pipeline->descriptor_sets[i];
		descriptor_writes[write_idx].dstBinding = 0;
		descriptor_writes[write_idx].dstArrayElement = 0;
		descriptor_writes[write_idx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[write_idx].descriptorCount = 1;
		descriptor_writes[write_idx].pImageInfo = &image_info;
		++write_idx;

		vkUpdateDescriptorSets(pipeline->dev->handle, cnt_of_array(descriptor_writes), descriptor_writes, 0, NULL);
	}
}

//## static
/**
Creates the font textures.
*/
static void create_font_texture(_vlk_imgui_pipeline_t* pipeline)
//##
{
	ImGuiIO* io = igGetIO();

	// TODO : make sure fonts exist, fall back to default if doesn't - imgui will assert otherwise.
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/Cousine-Regular.ttf").data(), 12);
	ImFont* font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "fonts/DroidSans.ttf", 13, NULL, NULL);
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/Karla-Regular.ttf").data(), 12);
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/ProggyClean.ttf").data(), 12);
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/ProggyTiny.ttf").data(), 12);
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/Roboto-Medium.ttf").data(), 13);

	/* 
	Build and load the texture atlas into a texture
	(In the examples/ app this is usually done within the ImGui_ImplXXX_Init() function from one of the demo Renderer)
	*/
	int width, height;
	unsigned char* pixels = NULL;
	ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &pixels, &width, &height, NULL);

	/*
	At this point you've got the texture data and you need to upload that your your graphic system:
	After we have created the texture, store its pointer/identifier (_in whichever format your engine uses_) in 'io.Fonts->TexID'.
	This will be passed back to your via the renderer. Basically ImTextureID == void*. Read FAQ below for details about ImTextureID.
	*/
	_vlk_texture_create_info_t create_info;
	clear_struct(&create_info);
	create_info.data = (void*)pixels;
	create_info.width = width;
	create_info.height = height;
	create_info.size = (size_t)width * height * 4;
	//create_info.type = TEXTURE_TYPE_2D;

	_vlk_texture__construct(&pipeline->font_texture, pipeline->dev, &create_info);
	io->Fonts->TexID = (void*)pipeline->font_texture.image;

	/*
	Create sampler
	*/
	VkSamplerCreateInfo sampler_info;
	clear_struct(&sampler_info);
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;

	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	sampler_info.maxAnisotropy = 1.0f;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = -1000;
	sampler_info.maxLod = 1000;

	if (vkCreateSampler(pipeline->dev->handle, &sampler_info, NULL, &pipeline->font_texture_sampler) != VK_SUCCESS) 
	{
		log__fatal("Failed to create texture sampler.");
	}
}

//## static
/**
Creates the pipeline.
*/
static void create_pipeline(_vlk_imgui_pipeline_t* pipeline)
//##
{
	VkShaderModule vert_shader = _vlk_device__create_shader(pipeline->dev, "bin/shaders/imgui.vert.spv");
	VkShaderModule frag_shader = _vlk_device__create_shader(pipeline->dev, "bin/shaders/imgui.frag.spv");

	/*
	Shader stage creation
	*/
	VkPipelineShaderStageCreateInfo vert_shader_stage_info;
	clear_struct(&vert_shader_stage_info);
	vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_info.module = vert_shader;
	vert_shader_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo frag_shader_stage_info;
	clear_struct(&frag_shader_stage_info);
	frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_info.module = frag_shader;
	frag_shader_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vert_shader_stage_info, frag_shader_stage_info };

	/*
	Vertex input
	*/

	/* Binding for vertices */
	VkVertexInputBindingDescription vertex_binding;
	clear_struct(&vertex_binding);
	vertex_binding.binding = 0;
	vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertex_binding.stride = sizeof(ImDrawVert);

	/* Position attribute */
	VkVertexInputAttributeDescription pos_attr;
	clear_struct(&pos_attr);
	pos_attr.binding = vertex_binding.binding;
	pos_attr.format = VK_FORMAT_R32G32_SFLOAT;
	pos_attr.location = 0;
	pos_attr.offset = offsetof(ImDrawVert, pos);

	/* Tex-coord attribute */
	VkVertexInputAttributeDescription tex_coord_attr;
	clear_struct(&tex_coord_attr);
	tex_coord_attr.binding = vertex_binding.binding;
	tex_coord_attr.format = VK_FORMAT_R32G32_SFLOAT;
	tex_coord_attr.location = 1;
	tex_coord_attr.offset = offsetof(ImDrawVert, uv);

	/* Color attribute */
	VkVertexInputAttributeDescription color_attr;
	clear_struct(&color_attr);
	color_attr.binding = vertex_binding.binding;
	color_attr.format = VK_FORMAT_R8G8B8A8_UNORM;
	color_attr.location = 2;
	color_attr.offset = offsetof(ImDrawVert, col);

	VkVertexInputBindingDescription binding_descriptions[] = { vertex_binding };
	VkVertexInputAttributeDescription attribute_descriptions[] = { pos_attr, tex_coord_attr, color_attr };

	VkPipelineVertexInputStateCreateInfo vertex_input_info;
	clear_struct(&vertex_input_info);
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = cnt_of_array(binding_descriptions);
	vertex_input_info.vertexAttributeDescriptionCount = cnt_of_array(attribute_descriptions);
	vertex_input_info.pVertexBindingDescriptions = binding_descriptions;
	vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions;

	/*
	Input assembly
	*/
	VkPipelineInputAssemblyStateCreateInfo input_assembly;
	clear_struct(&input_assembly);
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	/*
	Viewport and scissor
	*/
	VkViewport viewport;
	clear_struct(&viewport);
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)pipeline->extent.width;
	viewport.height = (float)pipeline->extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	clear_struct(&scissor);
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = pipeline->extent;

	VkPipelineViewportStateCreateInfo viewport_state;
	clear_struct(&viewport_state);
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.pViewports = &viewport;
	viewport_state.scissorCount = 1;
	viewport_state.pScissors = &scissor;

	/*
	Rasterizer
	*/
	VkPipelineRasterizationStateCreateInfo rasterizer;
	clear_struct(&rasterizer);
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE; // No cull mode for imgui
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	/*
	Multisampling
	*/
	VkPipelineMultisampleStateCreateInfo multisampling;
	clear_struct(&multisampling);
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = NULL; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	/*
	Color blending
	*/
	VkPipelineColorBlendAttachmentState color_blend_attachment;
	clear_struct(&color_blend_attachment);
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_TRUE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blending;
	clear_struct(&color_blending);
	color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable = VK_FALSE;
	color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &color_blend_attachment;
	color_blending.blendConstants[0] = 0.0f; // Optional
	color_blending.blendConstants[1] = 0.0f; // Optional
	color_blending.blendConstants[2] = 0.0f; // Optional
	color_blending.blendConstants[3] = 0.0f; // Optional

	/*
	Depth/stencil
	*/
	VkPipelineDepthStencilStateCreateInfo depth_stencil;
	clear_struct(&depth_stencil);
	depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	/*
	Dynamic state
	*/
	VkDynamicState dynamic_states[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state;
	clear_struct(&dynamic_state);
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = cnt_of_array(dynamic_states);
	dynamic_state.pDynamicStates = dynamic_states;

	/*
	Pipeline
	*/
	VkGraphicsPipelineCreateInfo pipeline_info;
	clear_struct(&pipeline_info);
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shaderStages;

	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterizer;
	pipeline_info.pMultisampleState = &multisampling;
	pipeline_info.pDepthStencilState = &depth_stencil; // Optional
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.pDynamicState = &dynamic_state; // Optional

	pipeline_info.layout = pipeline->layout;
	pipeline_info.renderPass = pipeline->render_pass;
	pipeline_info.subpass = 0;

	pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipeline_info.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(pipeline->dev->handle, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &pipeline->handle) != VK_SUCCESS)
	{
		log__fatal("Failed to create pipeline.");
	}

	/*
	* Cleanup
	*/
	_vlk_device__destroy_shader(pipeline->dev, vert_shader);
	_vlk_device__destroy_shader(pipeline->dev, frag_shader);
}

//## static
/**
Creates the pipeline layout.
*/
static void create_pipeline_layout(_vlk_imgui_pipeline_t* pipeline)
//##
{
	VkDescriptorSetLayout set_layouts[1];
	memset(set_layouts, 0, sizeof(set_layouts));
	set_layouts[0] = pipeline->descriptor_layout;

	/*
	Push constants

	Using 'vec2 offset' and 'vec2 scale' instead of a full 3d projection matrix.
	*/
	VkPushConstantRange push_constants[1];
	memset(push_constants, 0, sizeof(push_constants));
	push_constants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	push_constants[0].offset = sizeof(float) * 0;
	push_constants[0].size = sizeof(float) * 4;

	/*
	Create the pipeline layout
	*/
	VkPipelineLayoutCreateInfo pipeline_layout_info;
	clear_struct(&pipeline_layout_info);
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = cnt_of_array(set_layouts);
	pipeline_layout_info.pSetLayouts = set_layouts;
	pipeline_layout_info.pushConstantRangeCount = cnt_of_array(push_constants);
	pipeline_layout_info.pPushConstantRanges = push_constants;

	if (vkCreatePipelineLayout(pipeline->dev->handle, &pipeline_layout_info, NULL, &pipeline->layout) != VK_SUCCESS)
	{
		log__fatal("Failed to create pipeline layout.");
	}
}

//## static
/**
Destroys buffers.
*/
static void destroy_buffers(_vlk_imgui_pipeline_t* pipeline)
//##
{
	for (uint32_t i = 0; i < pipeline->vertex_buffers.count; ++i)
	{
		if (pipeline->vertex_buffer_is_init.data[i])
		{
			_vlk_buffer__destruct(&pipeline->vertex_buffers.data[i]);
			pipeline->vertex_buffer_is_init.data[i] = FALSE;
		}

		if (pipeline->index_buffer_is_init.data[i])
		{
			_vlk_buffer__destruct(&pipeline->index_buffers.data[i]);
			pipeline->index_buffer_is_init.data[i] = FALSE;
		}
	}

	utl_array_destroy(&pipeline->vertex_buffers);
	utl_array_destroy(&pipeline->vertex_buffer_sizes);
	utl_array_destroy(&pipeline->vertex_buffer_is_init);
	utl_array_destroy(&pipeline->index_buffers);
	utl_array_destroy(&pipeline->index_buffer_sizes);
	utl_array_destroy(&pipeline->index_buffer_is_init);
}

//## static
/**
Destroys descriptor set layout.
*/
static void destroy_descriptor_layout(_vlk_imgui_pipeline_t* pipeline)
//##
{
	vkDestroyDescriptorSetLayout(pipeline->dev->handle, pipeline->descriptor_layout, NULL);
}

//## static
/**
Destroys descriptor pool.
*/
static void destroy_descriptor_pool(_vlk_imgui_pipeline_t* pipeline)
//##
{
	vkDestroyDescriptorPool(pipeline->dev->handle, pipeline->descriptor_pool, NULL);
}

//## static
/**
Destroys descriptor sets.
*/
static void destroy_descriptor_sets()
//##
{
	/*
	Descriptor sets are destroyed automatically when the parent descriptor pool
	is destroyed.
	*/
}

//## static
/**
Destroys font texture and sampler.
*/
static void destroy_font_texture(_vlk_imgui_pipeline_t* pipeline)
//##
{
	vkDestroySampler(pipeline->dev->handle, pipeline->font_texture_sampler, NULL);
	_vlk_texture__destruct(&pipeline->font_texture);
}

//## static
/**
Destroys the pipeline.
*/
static void destroy_pipeline(_vlk_imgui_pipeline_t* pipeline)
//##
{
	vkDestroyPipeline(pipeline->dev->handle, pipeline->handle, NULL);
}

//## static
/**
Destroys the pipeline layout.
*/
void destroy_pipeline_layout(_vlk_imgui_pipeline_t* pipeline)
//##
{
	vkDestroyPipelineLayout(pipeline->dev->handle, pipeline->layout, NULL);
}
