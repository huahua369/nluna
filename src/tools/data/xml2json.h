#ifndef __xml2json__h__
#define __xml2json__h__
#include <iostream>
#define RAPIDXML_STATIC_POOL_SIZE 4*1024
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#ifndef NJSON_H
//#define JSON_NOEXCEPTION
#include "json.hpp"
#if defined( NLOHMANN_JSON_HPP) || defined(INCLUDE_NLOHMANN_JSON_HPP_)
using njson = nlohmann::json;
#define NJSON_H
#endif
#endif
namespace hz
{
	namespace xj {
		static const char xml2json_text_additional_name[] = "#text";
		static const char xml2json_ctext_additional_name[] = "#ctext";
		static const std::string attribute_name_prefix = "@";
		static const char xml2json_comment_name_prefix[] = "!--";
		static const std::string xml2json_child_name = "[child]";
		static const std::string sort_name = "[sort]";
	};
	namespace xj
	{
		class obj_t
		{
		private:
			std::vector<uint64_t> _obj;
			std::vector<char> _data;
		public:
			obj_t()
			{
			}

			~obj_t()
			{
			}

		private:

		};

		// 替换字符串
		static std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value)
		{
			while (true)
			{
				std::string::size_type pos(0);
				if ((pos = str.find(old_value)) != std::string::npos)
					str.replace(pos, old_value.length(), new_value);
				else
					break;
			}
			return   str;
		}
#if 1
		/* Example:
		   <node_name attribute_name="attribute_value">value</node_name> ---> "node_name":{"#text":"value","@attribute_name":"attribute_value"}
		*/
		static const bool xml2json_numeric_support = false;
		/* Example:
		   xml2json_numeric_support = false:
		   <number>26.026</number>  ---> "number":"26.026"
		   xml2json_numeric_support = true:
		   <number>26.026</number>  ---> "number":26.026
		*/
		/* [End]   This part is configurable */

		// Avoided any namespace pollution.
		static bool xml2json_has_digits_only(const char* input, bool* hasDecimal, int len)
		{
			if (input == nullptr)
				return false;  // treat empty input as a string (probably will be an empty string)

			const char* runPtr = input;

			*hasDecimal = false;

			while (*runPtr != '\0' && len > 0)
			{
				if (*runPtr == '.')
				{
					if (!(*hasDecimal))
						*hasDecimal = true;
					else
						return false; // we found two dots - not a number
				}
				else if (isalpha(*runPtr))
				{
					return false;
				}
				runPtr++;
				len--;
			}

			return true;
		}
#if 1
		static void xml2json_to_array_form(const char* name, njson& jsvalue, njson& jsvalue_chd)
		{
			njson jsvalue_target = jsvalue[name];
			if (jsvalue_target.is_array())
			{
				jsvalue_target.push_back(jsvalue_chd);
				jsvalue[name] = jsvalue_target;
			}
			else
			{
				njson jsvalue_array;
				jsvalue_array.push_back(jsvalue_target);
				jsvalue_array.push_back(jsvalue_chd);
				jsvalue[name] = jsvalue_array;
			}
		}
		inline std::string get_next_ch(char a)
		{
			if (a > 'z' && a < 'A')
			{
				a = 'A';
			}
			std::string ret;
			ret.push_back(a);
			return ret;
		}
		static void xml2json_add_attributes(rapidxml::xml_node<>* xmlnode, njson& jsvalue, bool is_sort)
		{
			rapidxml::xml_attribute<>* myattr;
			njson tem;
			std::vector<std::string> ks;
			std::vector<njson> vs;
			for (myattr = xmlnode->first_attribute(); myattr; myattr = myattr->next_attribute())
			{
				std::string jv;
				std::string jn(myattr->name(), myattr->name_size());
				if (xml2json_numeric_support)
				{
					bool hasDecimal;
					if (xml2json_has_digits_only(myattr->value(), &hasDecimal, myattr->value_size()) == false)
					{
						jv.assign(myattr->value(), myattr->value_size());
					}
					else
					{
						if (hasDecimal)
						{
							double value = std::strtod(myattr->value(), nullptr);
							jv = value;
						}
						else
						{
							long int value = std::strtol(myattr->value(), nullptr, 0);
							jv = value;
						}
					}
				}
				else
				{
					jv.assign(myattr->value(), myattr->value_size());
				}
				ks.push_back(jn);
				vs.push_back(jv);
			}
			if (ks.size())
			{
				std::string str = std::to_string(ks.size() - 1);
				size_t i = 0, cs = str.size();
				for (auto& it : ks)
				{
					std::string idx = std::to_string(i);
					int64_t idxs = cs - idx.size();
					if (idxs > 0)
					{
						idx.insert(idx.begin(), idxs, '0');
					}
					idx = attribute_name_prefix + it;
					jsvalue[idx] = vs[i++];
					if (is_sort)
						jsvalue[sort_name].push_back(idx);
				}
			}
		}

		static void xml2json_traverse_node(rapidxml::xml_node<>* xmlnode, njson& jsvalue, bool is_sort)
		{
			njson jsvalue_chd = njson::object();
			jsvalue = njson::object();
			rapidxml::xml_node<>* xmlnode_chd;

			// classified discussion:
			if ((xmlnode->type() == rapidxml::node_data || xmlnode->type() == rapidxml::node_cdata) && xmlnode->value())
			{
				// case: pure_text
				jsvalue = std::string(xmlnode->value(), xmlnode->value_size());  // then addmember("#text" , jsvalue, allocator)
			}
			else if (xmlnode->type() == rapidxml::node_element)
			{
				if (xmlnode->first_attribute())
				{
					if (xmlnode->first_node() && xmlnode->first_node()->type() == rapidxml::node_data && count_children(xmlnode) == 1)
					{
						// case: <e attr="xxx">text</e>
						jsvalue[xml2json_text_additional_name] = std::string(xmlnode->first_node()->value(), xmlnode->first_node()->value_size());
						xml2json_add_attributes(xmlnode, jsvalue, is_sort);
						return;
					}
					else
					{
						// case: <e attr="xxx">...</e>
						xml2json_add_attributes(xmlnode, jsvalue, is_sort);
					}
				}
				else
				{
					if (!xmlnode->first_node())
					{
						// case: <e />
						jsvalue = njson::object();
						return;
					}
					else if (xmlnode->first_node()->type() == rapidxml::node_data && count_children(xmlnode) == 1)
					{
						// case: <e>text</e>
						if (xml2json_numeric_support == false)
						{
							jsvalue = std::string_view(xmlnode->first_node()->value(), xmlnode->first_node()->value_size());
						}
						else
						{
							bool hasDecimal;
							if (xml2json_has_digits_only(xmlnode->first_node()->value(), &hasDecimal, xmlnode->first_node()->value_size()) == false)
							{
								jsvalue = std::string(xmlnode->first_node()->value(), xmlnode->first_node()->value_size());
							}
							else
							{
								if (hasDecimal)
								{
									double value = std::strtod(xmlnode->first_node()->value(), nullptr);
									jsvalue = value;
								}
								else
								{
									long int value = std::strtol(xmlnode->first_node()->value(), nullptr, 0);
									jsvalue = value;
								}
							}
						}
						return;
					}
				}
				if (xmlnode->first_node())
				{
					// case: complex else...
					std::map<std::string, int> name_count;
					njson chd;
					std::vector<std::pair<const char*, rapidxml::xml_node<>*>> vchd;
					std::vector<njson> tems;
					tems.reserve(100);
					vchd.reserve(100);
					//static int inc = 0;
					//int ic = 0;
					auto& o = jsvalue[xml2json_child_name];
					for (xmlnode_chd = xmlnode->first_node(); xmlnode_chd; xmlnode_chd = xmlnode_chd->next_sibling())
					{
						std::string current_name;
						std::string name_ptr;
						njson jn;
						if (xmlnode_chd->type() == rapidxml::node_data || xmlnode_chd->type() == rapidxml::node_cdata)
						{
							current_name = xml2json_text_additional_name;
							//name_count[current_name]++;
							name_ptr = std::string_view(current_name.c_str());
						}
						else if (xmlnode_chd->type() == rapidxml::node_element)
						{
							current_name = xmlnode_chd->name();
							//name_count[current_name]++;
							name_ptr = std::string_view(xmlnode_chd->name(), xmlnode_chd->name_size());
						}
						//vchd.push_back(std::pair<const char*, rapidxml::xml_node<>*>(name_ptr, xmlnode_chd));
						xml2json_traverse_node(xmlnode_chd, jsvalue_chd, is_sort);
						njson itv;
						itv[name_ptr].swap(jsvalue_chd);
						o.push_back(itv);
						//ic++;
					}
					//if (name_count.size())
					{
					}
					//o.swap(tems);
					//inc = std::max(ic, inc);
					//printf("max:\t%d\n", inc);
				}
			}
			else
			{
				std::cerr << "err data!!" << std::endl;
			}
		}
#endif
		// !x2j
#if 1

		static void json2xml_traverse_node(const njson& jsvalue, rapidxml::xml_node<>* xmlnode, rapidxml::xml_document<>* doc, const std::string& ak = "")
		{
			if (jsvalue.is_array())
			{
				for (auto& v : jsvalue)
				{
					auto istext = v.find(xml2json_text_additional_name);
					rapidxml::xml_node<>* xmlnode_chd = xmlnode;
					if (ak != xml2json_child_name)
					{
						xmlnode_chd = doc->allocate_node(rapidxml::node_element, doc->allocate_string(ak.c_str()), nullptr);
						xmlnode->append_node(xmlnode_chd);
					}
					json2xml_traverse_node(v, xmlnode_chd, doc, ak);
				}
			}
			else if (jsvalue.is_object())
			{
				njson jv = jsvalue;
				if (jv.find(sort_name) != jv.end())
				{
					auto& sn = jv[sort_name];
					for (auto& it : sn)
					{
						std::string k = toStr(it);
						size_t kapos = k.find(attribute_name_prefix);
						if (kapos != std::string::npos)
						{
							xmlnode->append_attribute(doc->allocate_attribute(doc->allocate_string(k.c_str() + kapos + 1), doc->allocate_string(toStr(jv[k]).c_str())));
							jv.erase(k);
						}
					}
					jv.erase(sort_name);
				}
				for (auto& [key, val] : jv.items())
				{
					rapidxml::xml_node<>* node = nullptr;
					bool iso = val.is_object(), isa = val.is_array();
					if (key != "")
					{
						size_t kapos = key.find(attribute_name_prefix);
						if (kapos != std::string::npos)
						{
							xmlnode->append_attribute(doc->allocate_attribute(doc->allocate_string(key.c_str() + kapos + 1), doc->allocate_string(toStr(val).c_str())));
						}
						else if (key == xml2json_text_additional_name)
						{
							auto sv = toStr(val);
							auto va = doc->allocate_string(sv.c_str(), sv.size());
							xmlnode->value(va, sv.size());
						}
						else if (!isa)
						{
							node = doc->allocate_node(rapidxml::node_element, doc->allocate_string(key.c_str()), iso ? nullptr : doc->allocate_string(toStr(val).c_str()));
							xmlnode->append_node(node);
						}
					}
					if (iso || isa)
					{
						json2xml_traverse_node(val, node ? node : xmlnode, doc, key);
					}
				}
			}
			else
			{
				printf("error: %s\n", jsvalue.dump(1).c_str());
			}
		}
#endif
		// !j2x

		// 实现
		static njson* xml2pjson(const std::string& xml_str, bool is_unordered = true)
		{
			njson* p = new njson();
			njson& js_doc = *p;
			rapidxml::xml_document<> xml_doc;
			try
			{
				const std::string& xb = xml_str;
				xml_doc.parse<rapidxml::parse_non_destructive>(const_cast<char*>(xb.c_str()));
				rapidxml::xml_node<>* xmlnode_chd;
				for (xmlnode_chd = xml_doc.first_node(); xmlnode_chd; xmlnode_chd = xmlnode_chd->next_sibling())
				{
					njson jsvalue_chd;
					xml2json_traverse_node(xmlnode_chd, jsvalue_chd, !is_unordered);
					js_doc[xmlnode_chd->name()].swap(jsvalue_chd);
				}
			}
			catch (const std::exception & e)
			{
				printf("error:\t%s\n", e.what());
				delete p;
				p = nullptr;
			}
			if (p && p->is_null())
			{
				delete p;
				p = nullptr;
			}
			return p;
		}
		static njson xml2json(const char* xml_str, bool is_unordered = true)
		{
			njson js_doc;
			rapidxml::xml_document<> xml_doc;
			try
			{
				auto xb = xml_str;
				xml_doc.parse<rapidxml::parse_non_destructive>(const_cast<char*>(xb));
				rapidxml::xml_node<>* xmlnode_chd;
				for (xmlnode_chd = xml_doc.first_node(); xmlnode_chd; xmlnode_chd = xmlnode_chd->next_sibling())
				{
					njson jsvalue_chd;
					xml2json_traverse_node(xmlnode_chd, jsvalue_chd, !is_unordered);
					js_doc[xmlnode_chd->name()].swap(jsvalue_chd);
				}
			}
			catch (const std::exception & e)
			{
				printf("error:\t%s\n", e.what());
			}
			return js_doc;
		}
		static std::string xml2json_str(const char* xml_str, int n, bool is_unordered = true)
		{
			njson js_doc = xml2json(xml_str, is_unordered);
			return n > 0 ? js_doc.dump(n) : js_doc.dump();
		}

		static std::string json2xml(const njson& js)
		{
			std::string ret;
			if (js.is_object())
			{
				try
				{
					rapidxml::xml_document<> doc;
					rapidxml::xml_node<>* rot = doc.allocate_node(rapidxml::node_pi, doc.allocate_string(R"(xml version="1.0" encoding="UTF-8" standalone="yes")"));
					doc.append_node(rot);
					njson js_doc;
					rapidxml::xml_node<>* node;
					for (auto& [k, v] : js.items())
					{
						node = doc.allocate_node(rapidxml::node_element, doc.allocate_string(k.c_str()), NULL);
						doc.append_node(node);
						json2xml_traverse_node(v, node, &doc);
					}
					rapidxml::print(std::back_inserter(ret), doc);
				}
				catch (const std::exception & e)
				{
					printf("%s\n", e.what());
				}
			}
			return ret;
		}
		static std::string json2xml_str(const std::string& js_str)
		{
			std::string ret;
			try
			{
				ret = json2xml(njson::parse(js_str.begin() + boms(js_str.c_str()), js_str.end()));
			}
			catch (const std::exception & e)
			{
				printf("%s\n", e.what());
			}
			return ret;
		}
		// 遍历json

#if 1

		static void json2obj_traverse_node(const njson& jsvalue, njson* onode, const std::string& ak = "")
		{
			if (jsvalue.is_array())
			{
				for (auto& v : jsvalue)
				{
					auto istext = v.find(xml2json_text_additional_name);
					njson* node_chd = onode;
					if (ak != xml2json_child_name)
					{
						node_chd = &(*onode)[ak];
					}
					json2obj_traverse_node(v, node_chd, ak);

				}
			}
			else if (jsvalue.is_object())
			{
				njson jv = jsvalue;
				if (jv.find(sort_name) != jv.end())
				{
					auto& sn = jv[sort_name];
					for (auto& it : sn)
					{
						std::string k = toStr(it);
						size_t kapos = k.find(attribute_name_prefix);
						if (kapos != std::string::npos)
						{
							(*onode)[k] = toStr(jv[k]).c_str();
							jv.erase(k);
						}
					}
					jv.erase(sort_name);
				}
				for (auto& [key, val] : jv.items())
				{
					njson* node = nullptr;
					bool iso = val.is_object(), isa = val.is_array();
					if (key != "")
					{
						size_t kapos = key.find(attribute_name_prefix);
						if (kapos != std::string::npos)
						{
							(*onode)[key] = toStr(val).c_str();
						}
						else if (key == xml2json_text_additional_name)
						{
							(*onode) = val;
						}
						else if (!isa)
						{
							node = &(*onode)[key];
							if (ak == xml2json_child_name)
							{
								njson n;
								node->push_back(n);
								node = &(*node->rbegin());
							}
							if (!iso)
							{
								*node = val;
							}
						}
					}
					if (iso || isa)
					{
						json2obj_traverse_node(val, node ? node : onode, key);
					}
				}
			}
			else
			{
				printf("error: %s\n", jsvalue.dump(1).c_str());
			}
		}
		static bool json2obj(const njson& js, njson& dst)
		{
			bool ret = false;
			if (js.is_object())
			{
				try
				{
					njson js_doc;
					njson* node;
					for (auto& [k, v] : js.items())
					{
						node = &dst[k];
						json2obj_traverse_node(v, node);
					}
					ret = true;
				}
				catch (const std::exception & e)
				{
					printf("%s\n", e.what());
				}
			}
			return ret;
		}
#endif
#endif

	}// !xj

}//!hz
#endif // !__xml2json__h__
