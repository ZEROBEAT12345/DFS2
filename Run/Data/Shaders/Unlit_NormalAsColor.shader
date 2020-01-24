<shader id="Unlit_NormalAsColor">

   <pass src="Data/Shaders/Unlit_NormalAsColor.hlsl">
      <vert entry="VertexFunction" />
      <frag entry="FragmentFunction" />
     
      <depth write="true" test="lequal" />

      <blend>
         <color op="add" src="srcAlpha" dst="invSrcAlpha" /> 
      </blend>
   </pass>
</shader>