<shader id="Lit_NormalMap_BRDF_IBL">
   <pass src="Data/Shaders/Lit_NormalMap_BRDF_IBL.hlsl">
      <vert entry="VertexFunction" />
      <frag entry="FragmentFunction" />

      <depth write="true" test="lequal" />

      <blend>
         <color op="add" src="srcAlpha" dst="invSrcAlpha" /> 
      </blend>
   </pass>
</shader>