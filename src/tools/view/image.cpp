

#include "image.h"
#include "file.h"
namespace hz
{
	// ************************************************************ //
	// GetImageData                                                 //
	//                                                              //
	// 从内存或文件加载图像数据解析									//
	// ************************************************************ //


	/*
	.hdr.bmp可选comp通道数
	quality为jpeg品质，1-100

	*/

	bool Image::load3Dtexture(const std::string& filename)
	{
#ifdef _GLI_HEAD_
		std::vector<char> td;
		File::read_binary_file(filename, td);
		if (td.empty())
		{
			return false;
		}
		gli::texture2d_array tex2DArray(gli::load(td.data(), td.size()));
		if (!tex2DArray.empty())
		{
			resize(static_cast<uint32_t>(tex2DArray.extent().x), static_cast<uint32_t>(tex2DArray.extent().y));
			layerCount = static_cast<uint32_t>(tex2DArray.layers());
			mipLevels = static_cast<uint32_t>(tex2DArray.levels());
			size_t imgsize = tex2DArray.size();
			_arrayimg.resize(imgsize);
			memcpy(&_arrayimg[0], tex2DArray.data(), static_cast<size_t>(tex2DArray.size()));
			// Setup buffer copy regions for each layer including all of it's miplevels

			size_t offset = 0;
			bufferCopyRegions.clear();
			clearArray();
			char* tem = (char*)&_arrayimg[0];
			std::string fn = File::getPath(filename.c_str(), File::pathdrive | File::pathdir | File::pathfname);
			for (uint32_t layer = 0; layer < layerCount; layer++)
			{
				for (uint32_t level = 0; level < mipLevels; level++)
				{
					std::vector<unsigned int>* img = new std::vector<unsigned int>;
					_array.push_back(img);
					BufferImageCopy bufferCopyRegion = {};
					bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					bufferCopyRegion.imageSubresource.mipLevel = level;
					bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
					bufferCopyRegion.imageSubresource.layerCount = 1;
					bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(tex2DArray[layer][level].extent().x);
					bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(tex2DArray[layer][level].extent().y);
					bufferCopyRegion.imageExtent.depth = 1;
					bufferCopyRegion.bufferOffset = offset;

					bufferCopyRegions.push_back(bufferCopyRegion);
					img->resize(bufferCopyRegion.imageExtent.width * bufferCopyRegion.imageExtent.height);
					memcpy(img->data(), tem + offset, bufferCopyRegion.imageExtent.width * bufferCopyRegion.imageExtent.height * 4);
					savefilename = fn + "/array_" + std::to_string(layer) + "_" + std::to_string(level) + ".png";

					//stbi_write_png_to_func(png_write_func, this, bufferCopyRegion.imageExtent.width, bufferCopyRegion.imageExtent.height, 4, img->data(), 0);
					// Increase offset into staging buffer for next level / face
					offset += tex2DArray[layer][level].size();

				}
			}
			return true;
		}
#endif
		return false;
	}

	int Image::saveImage(std::string fn, int comp, int quality)
	{
		if (_data.empty())
			return -1;
		LOCK_R(_locks);
		std::string ext = File::getPath(fn.c_str(), File::pathext);
		if (fn.find(":") == std::string::npos)
		{
			fn = File::getAP(fn);
		}

		if (ext == ".bmp")
		{
			if (comp == 4)
				comp = 3;
			getCompdata(comp);
			void* d = temp_data.empty() ? (void*)&_data[0] : (void*)&temp_data[0];
			return stbi_write_bmp(fn.c_str(), (int)width, (int)height, comp, d);
		}
		if (ext == ".tga")
		{
			return stbi_write_tga(fn.c_str(), (int)width, (int)height, requested_components, &_data[0]);
		}
		if (ext == ".jpg")
		{
			return stbi_write_jpg(fn.c_str(), (int)width, (int)height, requested_components, &_data[0], quality);
		}
		if (ext == ".hdr")
		{
			temp_data.resize(width * height * comp * sizeof(float));
			float* t = (float*)&temp_data[0];
			for (size_t y = 0; y < height; ++y)
			{
				for (size_t x = 0; x < width; ++x)
				{
					unsigned int c = _data[y * width + x];
					unsigned int R = GetRValue(c), G = GetGValue(c), B = GetBValue(c), A = c >> 24;
					*t++ = R / 255.0f;
					*t++ = G / 255.0f;
					*t++ = B / 255.0f;
					if (comp == 4)
						*t++ = A / 255.0f;
				}
			}
			return stbi_write_hdr(fn.c_str(), (int)width, (int)height, comp, (float*)&temp_data[0]);
		}
		savefilename = fn;
#ifdef LIBPNG
		if (ext == ".png")
		{
			hz::ImageLibpng ipng;
		}
		//return to_rgba();
#endif
		return stbi_write_png_to_func(png_write_func, this, (int)width, (int)height, comp, &_data[0], 0);
		//return stbi_write_png(fn.c_str(), (int)width, (int)height, requested_components, &_data[0], 0);
	}

	void Image::png_write_func(void* context, void* data, int size)
	{
		if (context)
		{
			Image* p = (Image*)context;
			File::save_binary_file(p->savefilename, (char*)data, size);
		}
	}

	bool hz::Image::GetImageData(const char* filename, size_t len)
	{
		LOCK_W(_locks);
		std::vector<char> file_data;
		std::string fn;
		if (len > 0)
		{
			file_data.resize(len);
			memcpy(&file_data[0], filename, len);
		}
		else
		{
			//hz::Timer t;
			fn = File::read_binary_file(filename, file_data);
			//int64_t s = t.elapsed();
			//LOGW("load %s file time: %lld ms%d\n", filename, s, (int)file_data.size());
			if (file_data.size() == 0 || fn.empty()) {
				return false;
			}
			_filename = filename;
		}
		int img_type = istupian((char*)&file_data[0], file_data.size());
		if (img_type == E_PNG)
		{
#ifdef LIBPNG
			hz::ImageLibpng ipng;
			ipng.load(&file_data);
			if (ipng.getHeight() && ipng.getWidth())
			{
				components = 4;
				resize(ipng.getWidth(), ipng.getHeight());
				memcpy(&_data[0], ipng.data(), ipng.datasize());
				return true;
			}
#endif
		}
		int tmp_width = 0, tmp_height = 0, tmp_z = 0, tmp_components = 0;
		int* delays = 0;
		unsigned char* image_data = 0;
		bool ret = false;
		if (img_type == E_GIF)
		{
			image_data = stbi_load_gif_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()),
				&delays, &tmp_width, &tmp_height, &tmp_z, &tmp_components, requested_components);
			_layers = tmp_z;
			_delays.resize(_layers);
			memcpy(_delays.data(), delays, sizeof(int) * _layers);
			stbi_image_free(delays);
			ret = tmp_z > 0;
		}
		else
		{
			// requested_components = 4;
			image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size())
				, &tmp_width, &tmp_height, &tmp_components, requested_components);
			if ((image_data == nullptr) ||
				(tmp_width <= 0) ||
				(tmp_height <= 0) ||
				(tmp_components <= 0)) {
				//std::cout << "Could not read image data!" << std::endl;
				return false;
			}
		}

		size_t size = (tmp_width) * (tmp_height) * (requested_components <= 0 ? tmp_components : requested_components);
		if (tmp_z > 0)
		{
			size *= tmp_z;
		}

		components = tmp_components;
		size_t bs = resize(tmp_width, tmp_height, tmp_z) * requested_components;
		memcpy(&_data[0], image_data, size);

		stbi_image_free(image_data);
		mipLevels = layerCount = 1;
		_levels_info.resize(mipLevels);
		_levels_info[0].width = width;
		_levels_info[0].height = height;
		_levels_info[0]._size = size;
		set_update();
		return true;
	}
}
