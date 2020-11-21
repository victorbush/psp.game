
function terrain_flat_10x10(x, y, z)
    
    

    test = {
        transform = { pos = { x, y, z } },
        static_model = { model = "terrain.obj" }
    }
    
    return test

end

world =
{
    entities =
    {

		{
			transform = {
				pos = { 0.0, 0.0, 0.0 }
			},
			static_model = {
				model = "craft_cargoA.obj"
			},
            physics = {
                mass = 10.0,
                inertia = 1.0
            },
            player = {}
		},
        {
			transform = {
				pos = { -5.0, 0.0, 0.0 },
                scale = { 5.0, 5.0, 5.0 }
			},
			static_model = {
				model = "rock_largeA.obj"
			}
		},
        {
			transform = {
				pos = { 0.0, 0.0, 10.0 },
                scale = { 1.0, 1.0, 1.0 }
			},
			static_model = {
				model = "hangar_largeA.obj"
			}
		},
        {
			transform = {
				pos = { -3.0, 0.0, 10.0 },
                scale = { 1.0, 1.0, 1.0 }
			},
			static_model = {
				model = "hangar_largeB.obj"
			}
		},
        {
			transform = {
				pos = { -6.0, 0.0, 10.0 },
                scale = { 1.0, 1.0, 1.0 }
			},
			static_model = {
				model = "hangar_roundA.obj"
			}
		},
        {
			transform = {
				pos = { -9.0, 0.0, 10.0 },
                scale = { 1.0, 1.0, 1.0 }
			},
			static_model = {
				model = "hangar_roundB.obj"
			}
		},
        {
			transform = {
				pos = { 0.0, 0.0, 5.0 },
                scale = { 1.0, 1.0, 1.0 }
			},
			static_model = {
				model = "barrels.obj"
			}
		},
        {
			transform = {
				pos = { 1.0, 0.0, 5.0 },
                scale = { 1.0, 1.0, 1.0 }
			},
			static_model = {
				model = "astronautA.obj"
			}
		},
        {
			transform = {
				pos = { 2.0, 0.0, 5.0 },
                scale = { 1.0, 1.0, 1.0 }
			},
			static_model = {
				model = "astronautB.obj"
			}
		}
    },
    
    geometry =
    {
    }
}

function add_flat_terrain (origin_x, num_x, origin_z, num_z)
    width = 10.0
    height = 10.0

    for x = 1, num_x, 1
    do
        for z = 1, num_z, 1
        do
            world.entities[#world.entities+1] = { 
                transform = { pos = {  origin_x + (x * width), 0.0, origin_z + (z * height) }, scale = { width, 1.0, height } }, 
                static_model = { model = "terrain.obj" } 
            }
        end
    end
end


add_flat_terrain(-50.0, 10, -50.0, 10)

--[[
foo = {}
foo[#foo+1]="bar"
foo[#foo+1]="baz"
--]]

--[[
 { transform = { pos = {  0.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  1.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  2.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  3.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  4.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  5.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  6.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  7.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  8.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  9.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = { 10.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        
        { transform = { pos = {  -1.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  -2.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  -3.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  -4.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  -5.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  -6.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  -7.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  -8.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = {  -9.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
        { transform = { pos = { -10.0, 0.0, 0.0 } }, static_model = { model = "terrain.obj" } },
--]]
