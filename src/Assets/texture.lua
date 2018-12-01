-- A simple scene with some miscellaneous geometry.
-- This file is very similar to nonhier.lua, but interposes
-- an additional transformation on the root node.  
-- The translation moves the scene, and the position of the camera
-- and lights have been modified accordingly.

green = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
gray = gr.material({0.5, 0.5, 0.5}, {0.1, 0.2, 0.1}, 3, 1)
dgray = gr.material({0.2, 0.2, 0.2}, {0.1, 0.2, 0.1}, 3, 1)
yellow = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 1)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 1)
blue = gr.material({0.1, 0.1, 0.3}, {0.1, 0.2, 0.1}, 3, 1)
red = gr.material({1.0, 0.1, 0.3}, {0.1, 0.2, 0.1}, 3, 1)
glass = gr.material({0.0, 0.0, 0.2}, {0.8, 0.8, 0.8}, 50, 0.0)

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0, 1)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, 1)


scene = gr.node( 'scene' )
scene:translate(0, -300, 0)
scene:rotate('x', 20)

s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
scene:add_child(s1)
s1:set_material(green)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
scene:add_child(s2)
s2:set_material(green)

-- s3 = gr.nh_sphere('s3', {0, -1800, -500}, 1000)
-- scene:add_child(s3)
-- s3:set_material(gray)

b1 = gr.nh_box('b1', {50, -125, 150}, 100)
scene:add_child(b1)
b1:set_material(mat4)

c1 = gr.nh_cylinder('c1', {0, 0, 0}, 8, 300)
scene:add_child(c1)
c1:set_material(mat4)
c1:rotate('z', -20);
c1:translate(0, 0, 150)

s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
scene:add_child(s4)
s4:set_material(yellow)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene:add_child(s5)
s5:set_material(green)

water = gr.mesh('water', 'plane.obj' )
scene:add_child(water)
water:set_material(glass)
water:set_normalmap("sea_normal.png")
water:scale(500, 40, 500)
water:translate(0, 50, 0)

test = gr.cube('test')
scene:add_child(test)
test:set_material(dgray)
test:scale(1000, 1, 1000)
test:translate(-500, -100, -500)

-- plane = gr.mesh('plane', 'plane.obj' )
-- scene:add_child(plane)
-- plane:set_material(gray)
-- plane:scale(500, 1, 500)
-- plane:rotate('x', 90)
-- plane:translate(0, 200, -500)

testb = gr.cube('testb')
scene:add_child(testb)
testb:set_material(gray)
testb:scale(1000, 500, 1)
testb:translate(-500, -100, -500)

-- planel = gr.mesh('planel', 'plane.obj' )
-- scene:add_child(planel)
-- planel:set_material(red)
-- planel:scale(500, 1, 500)
-- planel:rotate('x', 90)
-- planel:rotate('y', 90)
-- planel:translate(-500, 200, 0)

testb1 = gr.cube('testb1')
scene:add_child(testb1)
testb1:set_material(red)
testb1:scale(1, 500, 1000)
testb1:translate(-500, -100, -500)

-- planer = gr.mesh('planer', 'plane.obj' )
-- scene:add_child(planer)
-- planer:set_material(blue)
-- planer:scale(500, 1, 500)
-- planer:rotate('x', 90)
-- planer:rotate('y', -90)
-- planer:translate(500, 200, 0)

testb2 = gr.cube('testb2')
scene:add_child(testb2)
testb2:set_material(blue)
testb2:scale(1, 500, 1000)
testb2:translate(500, -100, -500)

arc = gr.node('arc')
scene:add_child(arc)
arc:scale(30, 30, 30)
arc:rotate('y', -20)
arc:translate(-100, 0, 100)

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
steldodec:set_material(yellow)
scene:add_child(steldodec)
steldodec:translate(0, 100, 200)


white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'texture.png', 
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
	  {0, -275, 1000}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
