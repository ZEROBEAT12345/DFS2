<shader id="Unlit_Effect_Tonemap">
   <pass >
      <vert entry="VertexFunction" src="Data/Shaders/Unlit_Effect_Tonemap.hlsl"/>
      <frag entry="FragmentFunction" src="Data/Shaders/Unlit_Effect_Tonemap.hlsl"/>

      <depth write="true" test="lequal" />

      <blend>
         <color op="add" src="srcAlpha" dst="invSrcAlpha" /> 
      </blend>
   </pass>
</shader>