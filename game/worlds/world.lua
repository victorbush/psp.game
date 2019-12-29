world =
{
    entities =
    {
        {
            transform = {
                pos = { -10.0, 0.0, -10.0 }
            },
            static_model = 
            {
                model = "models/cube/cube.obj",
                material = "models/cube/cube.mat.lua"
            }
        },
		{
			transform = {
				pos = { 0.0, 0.0, -5.0 }
			},
			static_model = {
				model = "models/cube/cube.obj",
				material = "models/cube/cube.mat.lua"
			}
		}
    },
    
    geometry =
    {
        {
            type = "plane",
            verts = {
                -10.0, -5.0, 10.0,
                10.0, -5.0, 10.0,
                10.0, -5.0, -10.0,
                -10.0, -5.0, -10.0
            },
            material = "materials/wood_floor.mat.lua"
        }
    }
}