

import graphics from aroma

vert = [[
  precision mediump float;

  uniform vec4 C;
  uniform mat4 PMatrix;
  uniform bool texturing;

  attribute vec2 P; // vertex
  attribute vec2 T; // texture coords

  varying lowp vec4 vColor;
  varying mediump vec2 vTex;

  void main(void) {
    if (texturing) vTex = T;
    vColor = C;
    gl_Position = PMatrix * vec4(P, 0.0, 1.0);
  }
]]

frag = [[
  precision mediump float;

  uniform sampler2D tex;
  uniform bool texturing;

  varying lowp vec4 vColor;
  varying mediump vec2 vTex;

  void main(void) {
    if (texturing) {
      gl_FragColor = texture2D(tex, vTex) * vColor;
    } else {
      gl_FragColor = vColor;
    }
  }
]]

graphics.setDefaultShader graphics.newShader vert, frag

