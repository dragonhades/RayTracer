-- A simple scene with some miscellaneous geometry.
-- This file is very similar to nonhier.lua, but interposes
-- an additional transformation on the root node.  
-- The translation moves the scene, and the position of the camera
-- and lights have been modified accordingly.

green = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
gray = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 8, 1)
dgray = gr.material({0.2, 0.2, 0.2}, {0.1, 0.1, 0.1}, 5, 1)
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
ground:set_material(nvidia_green)
ground:scale(1, 1/1000.0, 1)
ground:translate(-500, -200, -500) ---500, -100, -500

testb = gr.mesh('testb', 'plane.obj')
scene:add_child(testb)
testb:set_material(gray)
testb:set_texture("logo_left.png")
testb:scale(1000, 40, 600)
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
-- side
-- card:rotate('x', 50)
-- card:rotate('y', 45)
-- card:rotate('z', 20)

-- real
card:rotate('x', -50)
-- card:rotate('x', -100)
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

-- card:add_child(outbody)

subtract_body = gr.cube('subtract_body')
subtract_body:set_material(red)
subtract_body:scale(500, 300, 70)
subtract_body:translate(0, -60, 0)

-- card:add_child(subtract_body)


shell = gr.construct('shell', 'difference')

shell:add_child(outbody)
shell:add_child(subtract_body)

subtract_body2 = gr.cube('subtract_body2')
subtract_body2:set_material(red)
subtract_body2:scale(250, 240, 15)
subtract_body2:translate(125, -10, 69.5)

-- card:add_child(subtract_body2)


shell2 = gr.construct('shell2', 'difference')
shell2:add_child(shell)
shell2:add_child(subtract_body2)

card:add_child(shell2)


sidebar1 = gr.cube('sidebar1')
sidebar1:set_material(silver)
card:add_child(sidebar1)
sidebar1:scale(120, 5, 10)
sidebar1:translate(180, -20, 25)

sidebar2 = gr.cube('sidebar2')
sidebar2:set_material(silver)
sidebar2:scale(120, 5, 30)
sidebar2:rotate('y', -135)
sidebar2:translate(190, -20, 25)

sidebar_c1 = gr.construct('sidebar_c1', 'difference')

subtract_sidebar21 = gr.cube('subtract_sidebar21')
subtract_sidebar21:set_material(red)
subtract_sidebar21:scale(120, 15, 50)
subtract_sidebar21:translate(80, -25, 70)

sidebar_c1:add_child(sidebar2)
sidebar_c1:add_child(subtract_sidebar21)

subtract_sidebar22 = gr.cube('subtract_sidebar22')
subtract_sidebar22:set_material(red)
subtract_sidebar22:scale(120, 15, 50)
subtract_sidebar22:translate(80, -25, -25)

sidebar_c2 = gr.construct('sidebar_c2', 'difference')

sidebar_c2:add_child(sidebar_c1)
sidebar_c2:add_child(subtract_sidebar22)

left_sidebar = gr.node('left_sidebar')
card:add_child(left_sidebar)
left_sidebar:add_child(sidebar_c2)

right_sidebar = gr.node('right_sidebar')
card:add_child(right_sidebar)
right_sidebar:add_child(sidebar_c2)
right_sidebar:rotate('z', 180)
right_sidebar:translate(470, -35, 0)

fan = gr.node('fan')

fan_out = gr.nh_cylinder('fan_out', {0, 0, 0}, 100, 10)
fan:add_child(fan_out)
fan_out:set_material(silver)
fan_out:rotate('x', 90)
fan_out:translate(120, 110, 75)

fan_in = gr.nh_cylinder('fan_in', {0, 0, 0}, 95, 10)
fan:add_child(fan_in)
fan_in:set_material(dgray)
fan_in:rotate('x', 90)
fan_in:translate(120, 110, 77)

fan_center = gr.nh_cylinder('fan_center', {0, 0, 0}, 35, 10)
fan:add_child(fan_center)
fan_center:set_material(silver)
fan_center:rotate('x', 90)
fan_center:translate(120, 110, 79)


left_fan = gr.node('left_fan')
card:add_child(left_fan)
left_fan:add_child(fan)

right_fan = gr.node('right_fan')
card:add_child(right_fan)
right_fan:add_child(fan)
right_fan:translate(265, 0, 0)


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
	-- 480, 270,
	-- 640, 360,
	1280, 720,
	-- 3840, 2160,
	  {0, -375, 1000}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
