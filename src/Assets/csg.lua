-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 1)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.1, 0.2, 0.1}, 10, 1)
mat32 = gr.material({0.5, 0.5, 0.5}, {0.1, 0.2, 0.1}, 10, 1)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.1)
glass = gr.material({0.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 50, 0.0)

scene_root = gr.node('root')

diff1 = gr.construct('diff1', 'difference')

b1 = gr.nh_box('b1', {-325, -175, -375}, 150)
diff1:add_child(b1)
b1:set_material(mat3)

b2 = gr.nh_box('b2', {-250, -140, -290}, 90)
diff1:add_child(b2)

diff2 = gr.construct('diff2', 'difference')
diff2:add_child(diff1)

b22 = gr.nh_box('b22', {-330, -70, -270}, 50)
diff2:add_child(b22)

union1 = gr.construct('union1', 'union')
scene_root:add_child(union1)
union1:rotate('y', -10)
union1:rotate('z', -20)
union1:translate(-90, -60, 0)

b12 = gr.nh_box('b12', {0, 0, 0}, 40)
b12:set_material(mat3)
b12:scale(3, 1, 5)
b12:translate(-230, -120, -440)

union1:add_child(b12)
union1:add_child(diff2)



csg2 = gr.construct('csg2', 'difference')
scene_root:add_child(csg2)

b3 = gr.nh_box('b3', {-325, -175, -375}, 150)
csg2:add_child(b3)
b3:set_material(mat3)

s4 = gr.nh_sphere('s4', {-240, -100, -280}, 90)
csg2:add_child(s4)

csg2:translate(500, 0, 0)

b5 = gr.cube('b5')
b5:set_material(mat32)
scene_root:add_child(b5)
b5:scale(160, 85, 15)
b5:translate(-240+500, -130, -320)

-- s4 = gr.nh_sphere('s4', {-100, 225, -300}, 50)
-- scene_root:add_child(s4)
-- s4:set_material(mat3)

-- s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
-- scene_root:add_child(s5)
-- s5:set_material(mat1)

-- A small stellated dodecahedron.

-- steldodec = gr.mesh( 'dodec', 'smstdodeca.obj' )
-- steldodec:set_material(mat3)
-- scene_root:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'csg.png', 
	-- 256, 256,
	-- 480, 270,
	-- 640, 360,
	1280, 720,
	-- 3840, 2160,

	  {0, 100, 550},
	  -- {0, 0, 750},

	   {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
