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

uniform mat4 modelViewProjectionMatrix;

void main()
{
    texVarying = tex;
    gl_Position = modelViewProjectionMatrix * position;
}
