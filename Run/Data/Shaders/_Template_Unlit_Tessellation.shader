<shader id="Template_Tessellation">
   <pass >
      <vert entry="VertexFunction" src="Data/Shaders/_Template_Unlit_Tessellation.hlsl"/>
      <frag entry="FragmentFunction" src="Data/Shaders/_Template_Unlit_Tessellation.hlsl"/>
      <hull entry="HullFunction" src="Data/Shaders/_Template_Unlit_Tessellation.hlsl"/>
      <domain entry="DomainFunction" src="Data/Shaders/_Template_Unlit_Tessellation.hlsl"/>

      <depth write="true" test="lequal" />

      <blend>
         <color op="add" src="srcAlpha" dst="invSrcAlpha" /> 
      </blend>
   </pass>
</shader>