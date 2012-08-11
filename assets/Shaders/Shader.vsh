//
//  Shader.vsh
//  rps
//
//  Created by Kyle Weicht on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

attribute vec4 position;
attribute vec2 tex;

varying mediump vec2 texVarying;

uniform mat4 worldMatrix;
uniform mat4 viewProjectionMatrix;

void main()
{
    texVarying = tex;
    vec4 world_pos = worldMatrix * position;
    gl_Position = viewProjectionMatrix * world_pos;
}
