-- A more macho version of simple_cows.py, in which cows aren't
-- spheres, they're cow-shaped polyhedral models.


-- We'll need an extra function that knows how to read Wavefront .OBJ
-- files.

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grass = gr.material({0.1, 0.2, 0.1}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.2, 0.3, 0.2}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.7, 0.3, 0.7}, 25)
mat4 = gr.material({0.6, 0.4, 0.0}, {0.2, 0.2, 0.2}, 15)
mat5 = gr.material({0.3, 0.1, 0.0}, {0.4, 0.2, 0.4}, 15)


-- ##############################################
-- the arch
-- ##############################################

inst = gr.node('inst')

arc = gr.node('arc')
inst:add_child(arc)
arc:translate(0, 0, -10)

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


-- #############################################
-- Read in the cow model from a separate file.
-- #############################################

cow_poly = gr.mesh('cow', 'cow.obj')
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)

cow_poly:translate(0.0, 3.637, 0.0)
cow_poly:scale(factor, factor, factor)
cow_poly:translate(0.0, -1.0, 0.0)

-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')
scene:rotate('X', 23)


s1 = gr.nh_sphere('s1', {0, 0, 0}, 40)
scene:add_child(s1)
s1:set_material(mat4)
s1:translate(-350, -50, -1200)

s2 = gr.nh_sphere('s2', {0, 0, 0}, 100)
scene:add_child(s2)
s2:set_material(mat5)
s2:translate(0, -200, -1200)

s4 = gr.nh_sphere('s4', {0, 0, 0}, 200)
scene:add_child(s4)
s4:set_material(mat4)
s4:translate(350, -50, -1200)

s5 = gr.nh_sphere('s5', {0, 0, 0}, 50)
scene:add_child(s5)
s5:set_material(mat5)
s5:translate(-200, 5, -1200)

-- the floor

plane = gr.mesh('plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(35, 35, 35)

-- Construct a central altar in the shape of a buckyball.  The
-- buckyball at the centre of the real Stonehenge was destroyed
-- in the great fire of 733 AD.

buckyball = gr.mesh( 'buckyball', 'buckyball.obj' )
scene:add_child(buckyball)
buckyball:set_material(stone)
buckyball:scale(1.5, 1.5, 1.5)

-- Use the instanced cow model to place some actual cows in the scene.
-- For convenience, do this in a loop.

cow_number = 1

for _, pt in pairs({
		      {{1,1.3,14}, 20},
		      {{5,1.3,-11}, 180},
		      {{-7.5,1.3,-3}, -60}}) do
   cow_instance = gr.node('cow' .. tostring(cow_number))
   scene:add_child(cow_instance)
   cow_instance:add_child(cow_poly)
   cow_instance:scale(1.4, 1.4, 1.4)
   cow_instance:rotate('Y', pt[2])
   cow_instance:translate(table.unpack(pt[1]))
   
   cow_number = cow_number + 1
end

-- Place a ring of arches.

for i = 1, 15 do
   an_arc = gr.node('arc' .. tostring(i))
   an_arc:translate(0, 0, (i-1) * -0.2)
   an_arc:rotate('Y', (i-1) * -62)
   an_arc:translate(0, (i-1) * 1.3, 0)
   scene:add_child(an_arc)
   an_arc:add_child(arc)
end

white_light = gr.light({700.0, 200.0, 250.0}, {0.2, 0.0, 0.05}, {1, 0, 0})
orange_light = gr.light({700.0, 100.0, 150.0}, {1.0, 165.0/255.0, 0.0}, {1, 0, 0})
cow_light = gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})
red_light = gr.light({700, 0, -1000}, {0.8, 0.0, 0.0}, {1, 0, 0})

gr.render(scene,
	  'sample.png', 500, 500,
	  {0, 4, 40}, {0, 0.25, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light, orange_light, red_light})
