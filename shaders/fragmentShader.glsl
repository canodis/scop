#version 330 core

out vec4 FragColor;

smooth in vec3 smoothcolor;
flat in vec3 flatcolor;
uniform bool u_IsSmooth;
uniform bool u_IsGrey;

void main()
{
	float grey = (0.2125 * flatcolor.x + 0.1154 * flatcolor.y + 0.021 * flatcolor.z);
	float sgrey = (0.2125 * smoothcolor.x + 0.1154 * smoothcolor.y + 0.021 * smoothcolor.z);
	if (u_IsGrey)
	{
		if (u_IsSmooth)
			FragColor = vec4(sgrey, sgrey, sgrey, 0.4f);
		else
			FragColor = vec4(grey, grey, grey, 0.4f);
	}
	else
	{
		if (u_IsSmooth)
			FragColor = vec4(smoothcolor, 1.0f);
		else
			FragColor = vec4(flatcolor, 1.0f);
	}
}