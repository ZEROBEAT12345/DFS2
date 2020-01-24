<shader id="IBL_Diffuse_Cubemap_Convolution">
   <pass src="Data/Shaders/IBL_Diffuse_Cubemap_Convolution.hlsl">
      <vert entry="VertexFunction" />
      <frag entry="FragmentFunction" />

      <depth write="false" test="lequal" />

      <blend>
         <color op="add" src="srcAlpha" dst="invSrcAlpha" /> 
      </blend>
   </pass>
</shader>