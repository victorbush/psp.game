/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"
#include "gpu/vlk/pipelines/vlk_private_pipelines.h"
#include "platforms/common.h"
#include "thirdparty/glus/GLUS/glus_matrix.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates buffers for the pipeline */
static void create_buffers(_vlk_plane_pipeline_t* pipeline);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_plane_pipeline__init
*/
void _vlk_plane_pipeline__init
	(
	_vlk_plane_pipeline_t*			pipeline,
	_vlk_dev_t*						device,
	VkPipelineLayout				layout,
	VkRenderPass					renderPass,
	VkExtent2D						extent
	)
{
	clear_struct(pipeline);
	pipeline->dev = device;

	create_buffers(pipeline);
	//createInfo = getCreateInfo();
	//createPipeline(createInfo);
}

/**
_vlk_plane_pipeline__term
*/
void _vlk_plane_pipeline__term(_vlk_plane_pipeline_t* pipeline)
{
	//destroyPipeline();
	//destroyBuffers();
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
create_buffers
*/
static void create_buffers(_vlk_plane_pipeline_t* pipeline)
{
	vec3_t vertex_data[4];

	/* Front left */
	vertex_data[0].x = 0.0f;
	vertex_data[0].y = 0.0f;
	vertex_data[0].z = 0.0f;

	/* Back left */
	vertex_data[1].x = 0.0f;
	vertex_data[1].y = 0.0f;
	vertex_data[1].z = -1.0f;

	/* Back right */
	vertex_data[2].x = 1.0f;
	vertex_data[2].y = 0.0f;
	vertex_data[2].z = -1.0f;

	/* Front right */
	vertex_data[3].x = 1.0f;
	vertex_data[3].y = 0.0f;
	vertex_data[3].z = 0.0f;

	uint16_t index_data[6] =
	{
		0, 2, 1,
		0, 3, 2
	};
	
	uint32_t vertex_data_size = sizeof(vertex_data);
	//size_t vertexDataSize = vertexData.size() * sizeof(glm::vec3);
	//_vertexBuffer = std::make_unique<VulkanBuffer>(_device, vertexDataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	//_vertexBuffer->Update(vertexData.data(), 0, vertexDataSize);
	FATAL("TODO");

	uint32_t index_data_size = sizeof(index_data);
	//size_t indexDataSize = indexData.size() * sizeof(uint32_t);
	//_indexBuffer = std::make_unique<VulkanBuffer>(_device, indexDataSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	//_indexBuffer->Update(indexData.data(), 0, indexDataSize);
	FATAL("TODO");
}