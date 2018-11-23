-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 1)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.1, 0.2, 0.1}, 10, 1)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.1)
glass = gr.material({0.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 50, 0.0)

scene_root = gr.node('root')

csg1 = gr.construct('csg1', 'difference')
scene_root:add_child(csg1)

b1 = gr.nh_box('b1', {-325, -175, -375}, 150)
csg1:add_child(b1)
b1:set_material(mat3)

s1 = gr.nh_sphere('s1', {-200, -50, -300}, 100)
csg1:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
scene_root:add_child(s2)
s2:set_material(glass)

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
scene_root:add_child(s3)
s3:set_material(mat2)

s4 = gr.nh_sphere('s4', {-100, 225, -300}, 50)
scene_root:add_child(s4)
s4:set_material(mat3)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene_root:add_child(s5)
s5:set_material(mat1)

-- A small stellated dodecahedron.

steldodec = gr.mesh( 'dodec', 'smstdodeca.obj' )
steldodec:set_material(mat3)
scene_root:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'csg.png', 
	-- 256, 256,
	480, 270,
	-- 640, 360,
	-- 1280, 720,
	-- 3840, 2160,

	  {0, 0, 550},
	  -- {0, 0, 750},

	   {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
