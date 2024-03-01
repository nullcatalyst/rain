export default {
    "device_queue": (device: GPUDevice) => device.queue,
    "device_create_bind_group_layout": (device: GPUDevice, descriptor: GPUBindGroupLayoutDescriptor) => device.createBindGroupLayout(descriptor),
    "device_create_bind_group": (device: GPUDevice, descriptor: GPUBindGroupDescriptor) => device.createBindGroup(descriptor),
    "device_create_buffer": (device: GPUDevice, descriptor: GPUBufferDescriptor) => device.createBuffer(descriptor),
    "device_create_command_encoder": (device: GPUDevice, descriptor: GPUCommandEncoderDescriptor) => device.createCommandEncoder(descriptor),
    "device_create_pipeline_layout": (device: GPUDevice, descriptor: GPUPipelineLayoutDescriptor) => device.createPipelineLayout(descriptor),
    "device_create_render_pipeline": (device: GPUDevice, descriptor: GPURenderPipelineDescriptor) => device.createRenderPipeline(descriptor),
    "device_create_sampler": (device: GPUDevice, descriptor: GPUSamplerDescriptor) => device.createSampler(descriptor),
    "device_create_shader_module": (device: GPUDevice, descriptor: GPUShaderModuleDescriptor) => device.createShaderModule(descriptor),
    "device_create_texture": (device: GPUDevice, descriptor: GPUTextureDescriptor) => device.createTexture(descriptor),
    "texture_create_view": (texture: GPUTexture, descriptor: GPUTextureViewDescriptor) => texture.createView(descriptor),
};
