-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25,0.3, 1.33)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0,1.0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25,1.0)
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.8, 1.55)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)


U1 = gr.construct('U1', 'union')
scene:add_child(U1)

c1 = gr.cube('c1')
U1:add_child(c1)
--scene:add_child(c1)
c1:set_material(gold)
c1:translate(-3, 1, 0)
c1:scale(2, 2, 2)

c2 = gr.cube('c2')
U1:add_child(c2)
---scene:add_child(c2)
c2:set_material(gold)
c2:translate(-4.5, 1.2, 1.6)

--U1:translate(-3, 0, 0)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'cube_test.png', 512, 512,
{0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
{0.4, 0.4, 0.4}, {l1, l2})
