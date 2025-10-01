@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f
{
    var uv: vec2f = 2.0 * in.UV - 1.0;
    uv.x *= (640.0 / 480.0);
    let dist: f32 = length(uv);
	let angle: f32 = atan2(uv.y, uv.x);
	let v : f32 = smoothstep( 0.3, 0.33, dist + 0.1 * cos(10.0 * angle));
	let col: vec3f = vec3f( v,v,v);
    return vec4f(col, 1.0);
}
