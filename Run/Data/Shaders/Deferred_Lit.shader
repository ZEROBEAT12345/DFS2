<shader id="Deferred_Lit">
   <pass src="Data/Shaders/Deferred_Lit.hlsl">
      <vert entry="VertexFunction" />
      <frag entry="FragmentFunction" />

      <depth write="true" test="lequal" />

      <blend>
         <color op="add" src="srcAlpha" dst="invSrcAlpha" /> 
      </blend>
   </pass>
</shader>