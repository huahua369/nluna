void test()
{
	auto tex0 = ut_texture::new_storage2d(texfn, dev, 0, true);
	//auto tex0 = ut_texture::new_storage2d("rio-2-blu-icon-5.png", dev, 0, true);
	if (!tex0)
	{
		return;
	}
	ut_texture* compute_targets[6] = {};
	// 创建3个输出image
	for (size_t i = 0; i < 5; i++)
	{
		compute_targets[i] = ut_texture::new_storage2d(dev, tex0->_width, tex0->_height, 0, true);
	}
	// 创建compute着色器实现高斯模糊
	ut_compute* gauss_comp = sh->new_compute(dev, { "gauss_blur.comp" });
	ut_compute* gauss_comp1 = sh->new_compute(dev, { "gauss_blur1.comp" });
	auto set1 = gauss_comp->new_set();			// pass 1 用
	auto set2 = gauss_comp->new_set();			// pass 2 用
	auto set_g1 = gauss_comp1->new_set();		// pass 1 用
	auto set_g2 = gauss_comp1->new_set();		// pass 2 用
	auto set_blend = blend_comp->new_set();
	set1->write_image("inputImage", tex0);
	set1->write_image("outputImage", compute_targets[0]);
	set2->write_image("inputImage", compute_targets[0]);
	set2->write_image("outputImage", compute_targets[1]);
	set_g1->write_image("inputImage", tex0);
	set_g1->write_image("outputImage", compute_targets[3]);
	set_g2->write_image("inputImage", compute_targets[3]);
	set_g2->write_image("outputImage", compute_targets[4]);
	set_blend->write_image("srcSampler", tex0);
	set_blend->write_image("tempSampler", compute_targets[1]);
	set_blend->write_image("outputImage", compute_targets[2]);
	glm::ivec3 gs = { tex0->_width / 16, tex0->_height / 16, 1 };
	Image ct[6];
	struct gauss_info {
		glm::vec2  dir;			// 水平过滤 { 1.0 / 256.0, 0 }，垂直过滤 { 0, 1.0 / 256.0 }，
		float glowFactor = 1.0;	// 颜色强度，水平过滤时glowFactor=1.0，垂直过滤时设置4.5之类
		float wt_normalize = (1.0 / (1.0 + 2.0 * (0.93 + 0.8 + 0.7 + 0.6 + 0.5 + 0.4 + 0.3 + 0.2 + 0.1)));
		float gauss[9] = { 0.93, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1 };
	}upc_gauss;
	const int NW = 5;
	struct gauss_info1 {
		glm::vec2  dir;			// H=x, V=y
		float blurScale = 1.5;
		float blurStrength = 1;
		float weight[NW] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
	}upc_gauss1;
	struct blend_info {
		glm::vec4  glow_color = { 1.0,0.0,0.0,3.6 };
		float outerGlow = 1.0;
		float innerGlow = 0.0;
	}upc_blend;
	double wtn = 0.0;
	//if (gr.size() < 9)
	{
		gr.resize(9);
	}
	//memcpy(upc_gauss1.gauss, a, sizeof(float) * 5 * 5);
	//for (size_t i = 0; i < 9; i++)
	//{
	//	upc_gauss.gauss[i] *= 0.1;
	//	wtn += upc_gauss.gauss[i];
	//	printf("%0.3f\t", upc_gauss.gauss[i]);
	//}
	//upc_gauss.wt_normalize = (1.0 / (1.0 + 2.0 * wtn));
	for (auto it : gr)
	{
	}
	printf("\n");
	cmd1->begin();
	{
		gauss_comp->bind(cmd1, set1);
		//upc.dir = { 1.0 / tex0->_width, 0 };
		upc_gauss.dir = { 1.0 , 0 };
		gauss_comp->set_push_constant(cmd1, &upc_gauss, sizeof(gauss_info), 0);
		cmd1->dispatch(gs.x, gs.y, gs.z);
		// 同步
		cmd1->barrier_image(compute_targets[0], 1, 1, ImageLayoutBarrier::ComputeGeneralRW, ImageLayoutBarrier::ComputeGeneralRW, false);
		gauss_comp->bind(cmd1, set2);
		//upc.dir = { 0, 1.0 / tex0->_height };
		upc_gauss.dir = { 0, 1.0 };
		upc_gauss.glowFactor = 1.2;
		gauss_comp->set_push_constant(cmd1, &upc_gauss, sizeof(gauss_info), 0);
		cmd1->dispatch(gs.x, gs.y, gs.z);
		// cmd1->barrier_mem_compute(true);//无关系不用同步
		upc_gauss1.dir = { 1.0,0 };
		gauss_comp1->bind(cmd1, set_g1);
		gauss_comp1->set_push_constant(cmd1, &upc_gauss1, sizeof(gauss_info1), 0);
		cmd1->dispatch(gs.x, gs.y, gs.z);
		cmd1->barrier_image(compute_targets[3], 1, 1, ImageLayoutBarrier::ComputeGeneralRW, ImageLayoutBarrier::ComputeGeneralRW, false);
		//cmd1->barrier_mem_compute(true);
		upc_gauss1.dir = { 0, 1.0 };
		gauss_comp1->bind(cmd1, set_g2);
		gauss_comp1->set_push_constant(cmd1, &upc_gauss1, sizeof(gauss_info1), 0);
		cmd1->dispatch(gs.x, gs.y, gs.z);
		//// 混合成光辉
		//blend_comp->bind(cmd1, set_blend);
		//blend_comp->set_push_constant(cmd1, &upc_blend, sizeof(blend_info), 0);
		//cmd1->dispatch(gs.x, gs.y, gs.z);
	}
	cmd1->end();
	{
		print_time pts("submit");
		sub->submit(sub->queue_c(), subptr, true);
	}
	tex0->save2Image(&ct[0]);
	compute_targets[1]->save2Image(&ct[1]);
	compute_targets[3]->save2Image(&ct[2]);
	compute_targets[4]->save2Image(&ct[3]);
	for (size_t i = 0; i < 4; i++)
	{
		ct[i].saveImage("temp/compute_pass" + std::to_string(i) + ".png");
	}
	printf("done\n");
	return;
}
