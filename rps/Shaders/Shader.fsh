//
//  Shader.fsh
//  rps
//
//  Created by Kyle Weicht on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

varying mediump vec2 texVarying;

uniform sampler2D diffuseTexture;
uniform mediump vec4 color;

void main()
{
    mediump vec4 texColor = texture2D(diffuseTexture, texVarying);
    gl_FragColor = color*texColor;
}
