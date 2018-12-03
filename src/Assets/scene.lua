-- A simple scene with some miscellaneous geometry.
-- This file is very similar to nonhier.lua, but interposes
-- an additional transformation on the root node.  
-- The translation moves the scene, and the position of the camera
-- and lights have been modified accordingly.

green = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
gray = gr.material({0.5, 0.5, 0.5}, {0.4, 0.4, 0.4}, 8, 1)
dgray = gr.material({0.2, 0.2, 0.2}, {0.3, 0.3, 0.3}, 10, 1)
yellow = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 1)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 1)
blue = gr.material({0.1, 0.1, 0.3}, {0.4, 0.4, 0.4}, 8, 1)
red = gr.material({1.0, 0.1, 0.3}, {0.4, 0.4, 0.4}, 8, 1)
glass = gr.material({0.0, 0.0, 0.2}, {0.8, 0.8, 0.8}, 50, 0.0)

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0, 1)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 1)


scene = gr.node( 'scene' )
scene:translate(0, -300, 0)
scene:rotate('x', 20)

-- s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
-- scene:add_child(s1)
-- s1:set_material(green)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
scene:add_child(s2)
s2:set_material(green)

-- s3 = gr.nh_sphere('s3', {0, -1800, -500}, 1000)
-- scene:add_child(s3)
-- s3:set_material(gray)

-- b1 = gr.nh_box('b1', {50, -125, 150}, 100)
-- scene:add_child(b1)
-- b1:set_material(mat4)

-- c1 = gr.nh_cylinder('c1', {0, 0, 0}, 8, 300)
-- scene:add_child(c1)
-- c1:set_material(mat4)
-- c1:rotate('z', -20);
-- c1:translate(0, 0, 150)

-- s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
-- scene:add_child(s4)
-- s4:set_material(yellow)

-- s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
-- scene:add_child(s5)
-- s5:set_material(green)

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
testb:scale(1000, 40, 600)
testb:rotate('x', 90);
testb:translate(0, -200, -1000)

testbl = gr.cube('testbl')
scene:add_child(testbl)
testbl:set_material(red)
testbl:scale(1, 600, 1000)
testbl:translate(-500, -200, -500)

testbr = gr.cube('testbr')
scene:add_child(testbr)
testbr:set_material(blue)
testbr:scale(1, 600, 1000)
testbr:translate(500, -200, -500)



-- arc = gr.node('arc')
-- scene:add_child(arc)
-- arc:scale(30, 30, 30)
-- arc:rotate('y', -20)
-- arc:translate(-100, 0, 100)

-- p1 = gr.nh_box('p1', {0, 0, 0}, 1)
-- arc:add_child(p1)
-- p1:set_material(stone)
-- p1:scale(0.8, 4, 0.8)
-- p1:translate(-2.4, 0, -0.4)

-- p2 = gr.nh_box('p2', {0, 0, 0}, 1)
-- arc:add_child(p2)
-- p2:set_material(stone)
-- p2:scale(0.8, 4, 0.8)
-- p2:translate(1.6, 0, -0.4)

-- s = gr.nh_sphere('s', {0, 0, 0}, 1)
-- arc:add_child(s)
-- s:set_material(stone)
-- s:scale(4, 0.6, 0.6)
-- s:translate(0, 4, 0)

-- A small stellated dodecahedron.

-- steldodec = gr.mesh( 'dodec', 'smstdodeca.obj' )
-- steldodec:set_material(yellow)
-- scene:add_child(steldodec)
-- steldodec:translate(0, 100, 200)

white_lightlt = gr.light({-100.0, 500.0, 450.0}, {0.9, 0.9, 0.9}, {1, 0.003, 0})
white_lightlb = gr.light({-100.0, 500.0, 350.0}, {0.9, 0.9, 0.9}, {1, 0.003, 0})
white_light = gr.light({0.0, 500.0, 400}, {0.9, 0.9, 0.9}, {1, 0.0003, 0})
white_lightrt = gr.light({100.0, 500.0, 450.0}, {0.9, 0.9, 0.9}, {1, 0.003, 0})
white_lightrb = gr.light({100.0, 500.0, 350.0}, {0.9, 0.9, 0.9}, {1, 0.003, 0})
orange_light = gr.light({100.0, 200.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'scene.png', 
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
