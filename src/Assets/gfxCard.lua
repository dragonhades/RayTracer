-- A simple scene with some miscellaneous geometry.
-- This file is very similar to nonhier.lua, but interposes
-- an additional transformation on the root node.  
-- The translation moves the scene, and the position of the camera
-- and lights have been modified accordingly.

green = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
gray = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 8, 1)
dgray = gr.material({0.2, 0.2, 0.2}, {0.3, 0.3, 0.3}, 10, 1)
yellow = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 1)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 1)
blue = gr.material({0.1, 0.1, 0.3}, {0.4, 0.4, 0.4}, 8, 1)
red = gr.material({1.0, 0.1, 0.3}, {0.4, 0.4, 0.4}, 8, 1)
nvidia_green = gr.material({118/255, 185/255, 0.0}, {0.4, 0.4, 0.4}, 8, 1)
glass = gr.material({0.0, 0.0, 0.2}, {0.8, 0.8, 0.8}, 50, 0.0)
silver = gr.material({0.9, 0.9, 0.9}, {0.2, 0.2, 0.2}, 12, 1)

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0, 1)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 1)


scene = gr.node( 'scene' )
scene:translate(0, -300, 0)
scene:rotate('x', 20)

-- water = gr.mesh('water', 'plane.obj' )
-- scene:add_child(water)
-- water:set_material(glass)
-- water:set_normalmap("water_normal.png")
-- water:scale(500, 40, 500)
-- water:translate(0, 50, 0)

ground = gr.nh_box('ground', {0,0,0}, 1000)
scene:add_child(ground)
ground:set_material(dgray)
ground:scale(1, 1/1000.0, 1)
ground:translate(-500, -200, -500) ---500, -100, -500

testb = gr.mesh('testb', 'plane.obj')
scene:add_child(testb)
testb:set_material(gray)
-- testb:set_texture("Jensen_cut.png")
testb:scale(500, 40, 300)
testb:rotate('x', 90);
testb:translate(0, 100, -500)

-- testbl = gr.cube('testbl')
-- scene:add_child(testbl)
-- testbl:set_material(nvidia_green)
-- testbl:scale(1, 600, 1000)
-- testbl:translate(-500, -200, -500)

-- testbr = gr.cube('testbr')
-- scene:add_child(testbr)
-- testbr:set_material(nvidia_green)
-- testbr:scale(1, 600, 1000)
-- testbr:translate(500, -200, -500)

card = gr.node( 'card' )
scene:add_child(card)
-- card:rotate('x', 50)
-- card:rotate('y', 45)
-- card:rotate('z', 20)
card:rotate('x', -50)
card:rotate('y', 30)
card:rotate('z', 30)
card:translate(-100, -100, 400)

inbody = gr.cube('inbody')
inbody:set_material(dgray)
inbody:scale(500, 220, 70)

card:add_child(inbody)

outbody = gr.cube('outbody')
outbody:set_material(silver)
outbody:scale(510, 230, 80)
outbody:translate(-5, -5, -5)

subtract_body = gr.cube('subtract_body')
subtract_body:scale(500, 220, 70)
subtract_body:translate(0, -50, 0)

shell = gr.construct('shell', 'difference')

shell:add_child(outbody)
shell:add_child(subtract_body)

subtract_body2 = gr.cube('subtract_body2')
subtract_body2:scale(250, 220, 10)
-- subtract_body2:translate(0, 0, 0)

shell2 = gr.construct('shell2', 'difference')
shell2:add_child(shell)
shell2:add_child(subtract_body2)

card:add_child(shell2)


white_lightlt = gr.light({-100.0, 500.0, 450.0}, {0.9, 0.9, 0.9}, {1, 0.003, 0})
white_lightlb = gr.light({-100.0, 500.0, 350.0}, {0.9, 0.9, 0.9}, {1, 0.003, 0})
white_light = gr.light({0.0, 500.0, 400}, {0.9, 0.9, 0.9}, {1, 0.0, 0})
white_lightrt = gr.light({100.0, 500.0, 450.0}, {0.9, 0.9, 0.9}, {1, 0.003, 0})
white_lightrb = gr.light({100.0, 500.0, 350.0}, {0.9, 0.9, 0.9}, {1, 0.003, 0})
orange_light = gr.light({100.0, 200.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'card.png', 
	-- 256, 256,
	-- 512, 512,
	-- 1024, 1024,
	-- 1280, 1280,
	-- 2160, 2160,

		-- 256, 256,
	480, 270,
	-- 640, 360,
	-- 1280, 720,
	-- 3840, 2160,
	  {0, -375, 1000}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
