#ifndef IRENDERER_H
#define IRENDERER_H

#ifdef _WIN32
#pragma once
#endif

typedef struct renderer_studio_callback_s
{
}
renderer_studio_callback_t;

class IStudioRenderer
{
public:
	virtual void Init(renderer_studio_callback_t *pCallBack);
	virtual void RenderModel(void);
	virtual void RenderFinal(void);
};

class IRenderer : public IBaseInterface
{
public:
	virtual void Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave);
	virtual void Shutdown(void);
	virtual void LoadEngine(void);
	virtual void LoadClient(cl_exportfuncs_t *pExportFunc);
	virtual void ExitGame(int iResult);
};

#define RENDERER_API_VERSION "RENDERER_API_VERSION001"

#endif