#version 330
// precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_texture;
in vec4 colour;

void main()
{
    vec4 colourNew = texture2D(s_texture, v_texCoord);
	//  if(colour.a == 0.0) discard;
    gl_FragColor = colourNew;
}