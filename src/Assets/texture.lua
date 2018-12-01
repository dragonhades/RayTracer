-- A simple scene with some miscellaneous geometry.
-- This file is very similar to nonhier.lua, but interposes
-- an additional transformation on the root node.  
-- The translation moves the scene, and the position of the camera
-- and lights have been modified accordingly.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 1)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 1)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 1)
glass = gr.material({0.0, 0.0, 0.3}, {0.8, 0.8, 0.8}, 50, 0.0)

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0, 1)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 1)


scene = gr.node( 'scene' )
scene:rotate('x', 20)

s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
scene:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
scene:add_child(s2)
s2:set_material(mat1)

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
scene:add_child(s3)
s3:set_material(mat2)

b1 = gr.nh_box('b1', {-200, -125, 0}, 100)
scene:add_child(b1)
b1:set_material(mat4)

s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
scene:add_child(s4)
s4:set_material(mat3)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene:add_child(s5)
s5:set_material(mat1)

plane = gr.mesh('plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(glass)
plane:set_normalmap("water_normal.png")
plane:scale(800, 100, 800)
plane:translate(0, 50, 0)

arc = gr.node('arc')
scene:add_child(arc)
arc:scale(30, 30, 30)
arc:translate(-100, 0, 0)

p1 = gr.nh_box('p1', {0, 0, 0}, 1)
arc:add_child(p1)
p1:set_material(stone)
p1:scale(0.8, 4, 0.8)
p1:translate(-2.4, 0, -0.4)

p2 = gr.nh_box('p2', {0, 0, 0}, 1)
arc:add_child(p2)
p2:set_material(stone)
p2:scale(0.8, 4, 0.8)
p2:translate(1.6, 0, -0.4)

s = gr.nh_sphere('s', {0, 0, 0}, 1)
arc:add_child(s)
s:set_material(stone)
s:scale(4, 0.6, 0.6)
s:translate(0, 4, 0)

-- A small stellated dodecahedron.

steldodec = gr.mesh( 'dodec', 'smstdodeca.obj' )
steldodec:set_material(mat3)
scene:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.5, 0.7}, {1, 0, 0})

gr.render(scene, 'normal.png', 
	-- 256, 256,
	-- 512, 512,
	1024, 1024,
	  {0, 100, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
