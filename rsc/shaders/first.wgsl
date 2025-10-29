struct VertexInput {
	@location(0) position: vec2f,
	@location(1) color: vec3f,
    @location(2) UV: vec2f
};

struct VertexOutput {
	@builtin(position) position: vec4f,  // to be handled by the rasterizer
	@location(0) color: vec3f,
    @location(1) UV: vec2f,
};


@vertex
fn vs_main(in: VertexInput) -> VertexOutput
{
	var out: VertexOutput;
	// TODO: just change the vertex information...
    var doubled_vertex_pos = 2.0 * in.position; 
	out.position = vec4f(doubled_vertex_pos, 0.0, 1.0);
	out.color = in.color;
    out.UV = in.UV;
	return out;
}

@group(0) @binding(0) var<uniform> uTime: f32;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f
{
    var uv: vec2f = 2.0 * in.UV - 1.0;

	// TODO: need resolution uniform instead of hardcoding this
    uv.x *= (640.0 / 480.0);
    let dist: f32 = length(uv);
	let angle: f32 = atan2(uv.y, uv.x);
	let v : f32 = 1.0 - smoothstep( 0.3, 0.33, dist + 0.1 * cos(10.0 * angle - uTime));
	let col: vec3f = vec3f( v, 0.2, fract(uTime));
    return vec4f(col, 1.0);
}
