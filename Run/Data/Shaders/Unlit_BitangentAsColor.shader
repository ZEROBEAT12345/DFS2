<shader id="Unlit_BitangentAsColor">
   <pass>
      <vert entry="VF" src="Data/Shaders/Unlit_BitangentAsColor.hlsl"/>
      <frag entry="FragmentFunction" src="Data/Shaders/Unlit_BitangentAsColor.hlsl"/>

      <depth write="true" test="lequal" />

      <blend>
         <color op="add" src="srcAlpha" dst="invSrcAlpha" /> 
      </blend>
   </pass>
</shader>