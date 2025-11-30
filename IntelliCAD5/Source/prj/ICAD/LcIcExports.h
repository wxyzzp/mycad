#ifndef LC_IC_EXPORTS_HPP
#define LC_IC_EXPORTS_HPP

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

//External functions...
int lw_initialise(void* data);
int lw_terminate(void* data);
int lw_render_wireframe(void* data);
int lw_render_hidden(void* data);
int lw_render_preview(void* data);
int lw_render_full(void* data);
int lw_edit_mtl(void* data);
int lw_edit_lights(void* data);
int lw_edit_bg(void* data);
int lw_options(void* data);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LC_IC_EXPORTS_HPP
