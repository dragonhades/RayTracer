-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 1)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.1, 0.2, 0.1}, 10, 1)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.1)
glass = gr.material({0.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 50, 0.0)

white = gr.material({1.0, 1.0, 1.0}, {0.5, 0.7, 0.5}, 25, 1)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {50, 0, -400}, 100)
scene_root:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
scene_root:add_child(s2)
s2:set_material(glass)

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
scene_root:add_child(s3)
s3:set_material(mat2)

b1 = gr.nh_box('b1', {0, 0, 0}, 100)
scene_root:add_child(b1)
b1:rotate('y', 35)
b1:rotate('x', 23)
b1:translate(-75, -125, 60)
b1:set_material(glass)

s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
scene_root:add_child(s4)
s4:set_material(mat3)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene_root:add_child(s5)
s5:set_material(mat1)

-- plane = gr.mesh('plane', 'plane.obj' )
-- scene_root:add_child(plane)
-- plane:set_material(white)
-- plane:scale(200, 200, 200)
-- plane:rotate('x', 90)
-- plane:translate(0, 0, -700)

-- plane2 = gr.mesh('plane2', 'plane.obj' )
-- scene_root:add_child(plane2)
-- plane2:set_material(mat3)
-- plane2:scale(200, 200, 200)
-- plane2:rotate('x', 90)
-- plane2:translate(400, 0, -700)


-- A small stellated dodecahedron.

steldodec = gr.mesh( 'dodec', 'smstdodeca.obj' )
steldodec:set_material(mat3)
scene_root:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.7, 0.7}, {1, 0, 0})

gr.render(scene_root, 'refraction.png', 
	-- 256, 256,
	480, 270,
	-- 640, 360,
	-- 1280, 720,
	-- 3840, 2160,

	  {0, 0, 550},
	  -- {0, 0, 750},

	   {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
