#version 450

layout(set=0, binding=0) uniform sampler2D srcSampler;
layout(set=0, binding=0) uniform sampler2D tempSampler;

layout(set=0, binding=0) uniform u_UniformBuffer{  
  int fiterMode;
  int glowMode;
  float glowFactor;
  vec2 horizontalDir;
  vec2 verticalDir;
};

layout(location = 0) in vec2 stc;
layout(location = 0) out vec4 o_Color;

float Gauss[9] = { 0.93, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1 };																																					
void main ()																																													
{																																													
	vec4 outcolo_1;																																													
	if ((fiterMode < 2)) {																																													
	vec2 tc_2;																																													
	tc_2 = stc;																																													
	int i_3;																																													
	vec2 dir_4;																																													
	vec2 step_5;																																													
	vec4 c_6;																																													
	c_6 = (texture (srcSampler, stc) * 0.0994036);																																													
	step_5 = horizontalDir;																																													
	if ((fiterMode == 1)) 																																													
	{																																													
		step_5 = verticalDir;																																													
	};																																													
	dir_4 = step_5;																																													
	i_3 = 0;																																													
	for (; i_3 < 9; i_3++, dir_4 = (dir_4 + step_5)) 																																													
	{																																													
		c_6 = ((c_6 + (texture (srcSampler, (tc_2 + dir_4)) *(Gauss[i_3] * 0.0994036))) 																																													
		+ (texture (srcSampler, (tc_2 - dir_4)) * (Gauss[i_3] * 0.0994036)));																																													
	};																																													
	if ((fiterMode == 1)) 																																													
	{																																													
		c_6 = (c_6 * glowFactor);																																													
	};																																													
		outcolo_1 = c_6;																																													
	} else {																																													
	vec4 c_7;																																													
	vec4 tmpvar_8;																																													
	tmpvar_8 = texture (tempSampler, stc);																																													
	c_7 = tmpvar_8;																																													
	vec4 tmpvar_9;																																													
	tmpvar_9 = texture (srcSampler, stc);																																													
	if ((glowMode == 1)) 																																													
	{																																													
		c_7 = ((1.0 - ((1.0 - tmpvar_8) * (1.0 - tmpvar_9))) - tmpvar_9);																																													
	} else 																																													
	{																																													
		c_7 = (c_7 * tmpvar_9);																																													
	};																																													
		outcolo_1 = c_7;																																													
	};																																													
	o_Color = outcolo_1;																																													
}				