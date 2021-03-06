#version 330
in vec3 vNormal;
out vec4 fColor;

uniform vec3 selectedItemColor;
uniform bool isSelected;
uniform float selectedTransparentValue;
uniform vec4 itemColor;

uniform bool lightByNormals;

vec3 lightDirection = vec3(0,0,1);

void main()
{
    if(isSelected)
    {
	fColor = vec4(selectedItemColor, selectedTransparentValue);
    }
    else
    {
	if(lightByNormals)
	    fColor = vec4(dot(vNormal, lightDirection) * itemColor.rgb, itemColor.a);
	else
	    fColor = itemColor;
    }
}
