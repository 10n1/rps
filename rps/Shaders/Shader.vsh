//
//  Shader.vsh
//  rps
//
//  Created by Kyle Weicht on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

attribute vec4 position;
attribute vec2 tex;

varying lowp vec4 colorVarying;

uniform mat4 modelViewProjectionMatrix;

void main()
{
    colorVarying = vec4(tex.s, tex.t, 1.0, 1.0);
    gl_Position = modelViewProjectionMatrix * position;
}
