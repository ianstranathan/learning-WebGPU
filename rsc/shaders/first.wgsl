// /**
//  * A structure with fields labeled with vertex attribute locations can be used
//  * as input to the entry point of a shader.
//  */
// struct VertexInput {
// 	@location(0) position: vec2f,
// 	@location(1) color: vec3f,
// };

// /**
//  * A structure with fields labeled with builtins and locations can also be used
//  * as *output* of the vertex shader, which is also the input of the fragment
//  * shader.
//  */
// struct VertexOutput {
// 	@builtin(position) position: vec4f,
// 	// The location here does not refer to a vertex attribute, it just means
// 	// that this field must be handled by the rasterizer.
// 	// (It can also refer to another field of another struct that would be used
// 	// as input to the fragment shader.)
// 	@location(0) color: vec3f,
// };

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

/**
 * A structure holding the value of our uniforms
 */
struct MyUniforms {
	color: vec4f,
	time: f32,
};

// Instead of the simple uTime variable, our uniform variable is a struct
@group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;

// @vertex
// fn vs_main(in: VertexInput) -> VertexOutput {
// 	var out: VertexOutput;
// 	let ratio = 640.0 / 480.0;

// 	// We now move the scene depending on the time!
// 	var offset = vec2f(-0.6875, -0.463);
// 	offset += 0.3 * vec2f(cos(uMyUniforms.time), sin(uMyUniforms.time));

// 	out.position = vec4f(in.position.x + offset.x, (in.position.y + offset.y) * ratio, 0.0, 1.0);
// 	out.color = in.color;
// 	return out;
// }
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

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	// We multiply the scene's color with our global uniform (this is one
	// possible use of the color uniform, among many others).
	//let color = in.color * uMyUniforms.color.rgb;

	// We apply a gamma-correction to the color
	// We need to convert our input sRGB color into linear before the target
	// surface converts it back to sRGB.
	var uv: vec2f = 2.0 * in.UV - 1.0;
	let d: f32 = length(uv);
	let col: vec3f = vec3f(d, d, d);

	let t = u
		let col = vec3f( 0.5 * (sin( uMyUniforms) + 1.0), 0.5 * (cos( uMyUniforms) + 1.0), 0.);
	let linear_color = pow(col, vec3f(2.2));
	
	return vec4f(linear_color + vec3f(1., 0., 0.), 1.0);
}

// -------------------------------------------------------------------------------------

// struct VertexInput {
// 	@location(0) position: vec2f,
// 	@location(1) color: vec3f,
//     @location(2) UV: vec2f
// };

// struct VertexOutput {
// 	@builtin(position) position: vec4f,  // to be handled by the rasterizer
// 	@location(0) color: vec3f,
//     @location(1) UV: vec2f,
// };



// @vertex
// fn vs_main(in: VertexInput) -> VertexOutput
// {
// 	var out: VertexOutput;
// 	// TODO: just change the vertex information...
//     var doubled_vertex_pos = 2.0 * in.position; 
// 	out.position = vec4f(doubled_vertex_pos, 0.0, 1.0);
// 	out.color = in.color;
//     out.UV = in.UV;
// 	return out;
// }


// //@group(0) @binding(0) var<uniform> uTime: f32;

// struct MyUniforms {
// 	color: vec4f,
// 	time: f32,
// };

// // Instead of the simple uTime variable, our uniform variable is a struct
// @group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;


// @fragment
// fn fs_main(in: VertexOutput) -> @location(0) vec4f
// {
//     var uv: vec2f = 2.0 * in.UV - 1.0;

// 	// TODO: need resolution uniform instead of hardcoding this
//     uv.x *= (640.0 / 480.0);
//     let dist: f32 = length(uv);
// 	let angle: f32 = atan2(uv.y, uv.x);
// 	let v : f32 = 1.0 - smoothstep( 0.3, 0.33, dist + 0.1 * cos(10.0 * angle - uTime));
// 	//let col: vec3f = vec3f( v, 0.2, fract(uTime));
// 	let col: vec3f = vec3f( fract(uMyUniforms.time),
// 							fract(uMyUniforms.time), 0.0);
//     return vec4f(col, 1.0);
// }
