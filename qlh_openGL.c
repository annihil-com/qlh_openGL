#define _GNU_SOURCE
#include "ghf/ghf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <link.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

unsigned long curBindTexture;

void (*orig_glBindTexture)(GLenum target, GLuint texture);
void hook_glBindTexture(GLenum target, GLuint texture);

void (*orig_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void hook_glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

void (*orig_glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
void hook_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);

void *dlsym_hook(void *handle, const char *symbol);

typedef void*(*dlsym_t)(void *handle, const char *symbol);
dlsym_t orig_dlsym;

void __attribute__ ((constructor)) q2hack_init (void) 
{
	orig_dlsym = detourFunction((void*)dlsym, (void*)&dlsym_hook);
}

void *dlsym_hook(void *handle, const char *symbol)
{
	void *result = orig_dlsym(handle, symbol);

	if(strstr(symbol, "glBindTexture"))
	{
		orig_glBindTexture = result;
		return hook_glBindTexture;
	}
	else if(strstr(symbol, "glDrawElements"))
	{
		orig_glDrawElements = result;
		return hook_glDrawElements;
	}
	else if(strstr(symbol, "glReadPixels"))
	{
		orig_glReadPixels = result;
		return hook_glReadPixels;
	}

	return result;
}

void hook_glBindTexture(GLenum target, GLuint texture)
{
	/* Thanks to Sinner at www.aimbots.net for pointing this out */
	asm ("movl %%esi, %0;" : "=r"(curBindTexture));

	orig_glBindTexture(target, texture);
}

void hook_glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	if(curBindTexture > 0x1000)
	{
		//printf("%s\n", (char*)curBindTexture);

		if(strstr((char*)curBindTexture, "models/players") && !strstr((char*)curBindTexture, "icon"))
		{
			glEnable(GL_COLOR_MATERIAL);
			glDisableClientState(GL_COLOR_ARRAY);

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_TEXTURE_2D);

			glColor3ub(255, 0, 0);
		
			glDrawElements(mode, count, type, indices);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_TEXTURE_2D);

			glColor3ub(0, 255, 0);

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);
		
			glDrawElements(mode, count, type, indices);

			glEnableClientState(GL_COLOR_MATERIAL);
			glDisable(GL_BLEND);
		}
	}
	
	orig_glDrawElements(mode, count, type, indices);
}

void hook_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
	/* Don't do anything for blank PBSS :o */
	//orig_glReadPixels(x, y, width, height, format, type, pixels);
}
