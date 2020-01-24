<shader id="IBL_Specular_Cubemap_Pre_filter_Convolution">
   <pass src="Data/Shaders/IBL_Specular_Cubemap_Pre_filter_Convolution.hlsl">
      <vert entry="VertexFunction" />
      <frag entry="FragmentFunction" />

      <depth write="false" test="lequal" />

      <blend>
         <color op="add" src="srcAlpha" dst="invSrcAlpha" /> 
      </blend>
   </pass>
</shader>