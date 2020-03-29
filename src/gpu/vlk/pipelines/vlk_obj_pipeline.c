/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates the pipeline layout. */
static void create_layout(_vlk_obj_pipeline_t* pipeline);

/** Creates the pipeline */
static void create_pipeline(_vlk_obj_pipeline_t* pipeline);

/** Destroys the pipeline layout. */
static void destroy_layout(_vlk_obj_pipeline_t* pipeline);

/** Destroys the pipeline. */
static void destroy_pipeline(_vlk_obj_pipeline_t* pipeline);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_obj_pipeline__construct
	(
	_vlk_obj_pipeline_t*			pipeline,
	_vlk_dev_t*						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	)
{
	clear_struct(pipeline);
	pipeline->dev = device;
	pipeline->extent = extent;
	pipeline->render_pass = render_pass;

	create_layout(pipeline);
	create_pipeline(pipeline);
}

void _vlk_obj_pipeline__destruct(_vlk_obj_pipeline_t* pipeline)
{
	destroy_pipeline(pipeline);
	destroy_layout(pipeline);
}

void _vlk_obj_pipeline__bind(_vlk_obj_pipeline_t* pipeline, VkCommandBuffer cmd)
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void create_layout(_vlk_obj_pipeline_t * pipeline)
{
	VkDescriptorSetLayout set_layouts[2];
	memset(set_layouts, 0, sizeof(set_layouts));
	set_layouts[0] = pipeline->dev->per_view_layout.handle;
	set_layouts[1] = pipeline->dev->material_layout.handle;

	/*
	Push constants
	*/
	VkPushConstantRange pc_vert;
	clear_struct(&pc_vert);
	pc_vert.offset = 0;
	pc_vert.size = sizeof(_vlk_obj_push_constant_vertex_t);
	pc_vert.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	// TODO : size and offset must be a multiple of 4
	VkPushConstantRange push_constants[] =
	{
		pc_vert
	};

	/*
	Make sure push constant data fits. Minimum is 128 bytes. Any bigger and
	need to check if device supports.
	https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPushConstantRange.html
	*/
	//auto maxPushConst = _device.GetGPU().GetDeviceProperties().limits.maxPushConstantsSize;
	auto maxPushConst = 128;
	if (sizeof(_vlk_obj_push_constant_t) > maxPushConst)
	{
		log__fatal("OBJ push constant size greater than max allowed.");
	}

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
		log__fatal("Failed to create OBJ pipeline layout.");
	}
}

static void create_pipeline(_vlk_obj_pipeline_t* pipeline)
{
	VkShaderModule vert_shader = _vlk_device__create_shader(pipeline->dev, "bin/shaders/obj.vert.spv");
	VkShaderModule frag_shader = _vlk_device__create_shader(pipeline->dev, "bin/shaders/obj.frag.spv");

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
	vertex_binding.stride = sizeof(_vlk_static_mesh_vertex_t);

	/* Position attribute */
	VkVertexInputAttributeDescription pos_attr;
	clear_struct(&pos_attr);
	pos_attr.binding = 0;
	pos_attr.format = VK_FORMAT_R32G32B32_SFLOAT;
	pos_attr.location = 0;
	pos_attr.offset = 0;

	/* Normal attribute */
	VkVertexInputAttributeDescription normal_attr;
	clear_struct(&normal_attr);
	normal_attr.binding = 0;
	normal_attr.format = VK_FORMAT_R32G32B32_SFLOAT;
	normal_attr.location = 1;
	normal_attr.offset = offsetof(_vlk_static_mesh_vertex_t, normal);

	/* Tex-coord attribute */
	VkVertexInputAttributeDescription tex_coord_attr;
	clear_struct(&tex_coord_attr);
	tex_coord_attr.binding = 0;
	tex_coord_attr.format = VK_FORMAT_R32G32_SFLOAT;
	tex_coord_attr.location = 2;
	tex_coord_attr.offset = offsetof(_vlk_static_mesh_vertex_t, tex);

	VkVertexInputBindingDescription binding_descriptions[] = { vertex_binding };
	VkVertexInputAttributeDescription attribute_descriptions[] = { pos_attr, normal_attr, tex_coord_attr };

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
	rasterizer.cullMode = VK_CULL_MODE_NONE; // No culling for now
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
	color_blend_attachment.blendEnable = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

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
	depth_stencil.depthTestEnable = VK_TRUE;
	depth_stencil.depthWriteEnable = VK_TRUE;
	depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil.depthBoundsTestEnable = VK_FALSE;
	depth_stencil.minDepthBounds = 0.0f; // Optional
	depth_stencil.maxDepthBounds = 1.0f; // Optional
	depth_stencil.stencilTestEnable = VK_FALSE;
	//depth_stencil.front = {}; // Optional
	//depth_stencil.back = {}; // Optional

	/*
	Dynamic state
	*/
	//VkDynamicState dynamicStates[] =
	//{
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH,
	//};

	//VkPipelineDynamicStateCreateInfo dynamic_state;
	//clear_struct(&dynamic_state);
	//dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//dynamic_state.dynamicStateCount = 2;
	//dynamic_state.pDynamicStates = dynamicStates;

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
	pipeline_info.pDynamicState = NULL;// &dynamic_state; // Optional

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

void destroy_layout(_vlk_obj_pipeline_t* pipeline)
{
	vkDestroyPipelineLayout(pipeline->dev->handle, pipeline->layout, NULL);
}

static void destroy_pipeline(_vlk_obj_pipeline_t* pipeline)
{
	vkDestroyPipeline(pipeline->dev->handle, pipeline->handle, NULL);
}