//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			pUI->m_constAttenSlider->value(pUI->raytracer->getConstAtten());
			pUI->m_linearAttenSlider->value(pUI->raytracer->getLinearAtten());
			pUI->m_quadraticAttenSlider->value(pUI->raytracer->getQuadraticAtten());
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_ambientSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());

	pUI->raytracer->changeAmbient(double (((Fl_Slider *)o)->value()));
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_constAttenSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setConstAttenCoeff(double(((Fl_Slider *)o)->value()));
}
void TraceUI::cb_linearAttenSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setLinearAttenCoeff(double(((Fl_Slider *)o)->value()));
}
void TraceUI::cb_quadraticAttenSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setQuadraticAttenCoeff(double(((Fl_Slider *)o)->value()));
}

void TraceUI::cb_minIntensitySlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setMinIntensity(double(((Fl_Slider *)o)->value()));
}

void TraceUI::cb_superResSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->raytracer->setSuperRes(int(((Fl_Slider *)o)->value()));
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y, pUI->getDepth() );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_mainWindow = new Fl_Window(100, 40, 360, 235, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 360, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		// install slider ambient
		m_ambientSlider = new Fl_Value_Slider(10, 80, 180, 20, "Ambient");
		m_ambientSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientSlider->labelfont(FL_COURIER);
		m_ambientSlider->labelsize(12);
		m_ambientSlider->minimum(0);
		m_ambientSlider->maximum(1);
		m_ambientSlider->step(0.01);
		m_ambientSlider->value(1);
		m_ambientSlider->align(FL_ALIGN_RIGHT);
		m_ambientSlider->callback(cb_ambientSlides);

		// install slider const atten
		m_constAttenSlider = new Fl_Value_Slider(10, 105, 180, 20, "Const Attenuation");
		m_constAttenSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_constAttenSlider->type(FL_HOR_NICE_SLIDER);
		m_constAttenSlider->labelfont(FL_COURIER);
		m_constAttenSlider->labelsize(12);
		m_constAttenSlider->minimum(0);
		m_constAttenSlider->maximum(1);
		m_constAttenSlider->step(0.01);
		m_constAttenSlider->value(0.25);
		m_constAttenSlider->align(FL_ALIGN_RIGHT);
		m_constAttenSlider->callback(cb_constAttenSlides);

		// install slider linear atten
		m_linearAttenSlider = new Fl_Value_Slider(10, 130, 180, 20, "Linear Attenuation");
		m_linearAttenSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_linearAttenSlider->type(FL_HOR_NICE_SLIDER);
		m_linearAttenSlider->labelfont(FL_COURIER);
		m_linearAttenSlider->labelsize(12);
		m_linearAttenSlider->minimum(0);
		m_linearAttenSlider->maximum(1);
		m_linearAttenSlider->step(0.01);
		m_linearAttenSlider->value(0.25);
		m_linearAttenSlider->align(FL_ALIGN_RIGHT);
		m_linearAttenSlider->callback(cb_linearAttenSlides);

		// install slider quadratic atten
		m_quadraticAttenSlider = new Fl_Value_Slider(10, 155, 180, 20, "Quadratic Attenuation");
		m_quadraticAttenSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_quadraticAttenSlider->type(FL_HOR_NICE_SLIDER);
		m_quadraticAttenSlider->labelfont(FL_COURIER);
		m_quadraticAttenSlider->labelsize(12);
		m_quadraticAttenSlider->minimum(0);
		m_quadraticAttenSlider->maximum(1);
		m_quadraticAttenSlider->step(0.01);
		m_quadraticAttenSlider->value(0.5);
		m_quadraticAttenSlider->align(FL_ALIGN_RIGHT);
		m_quadraticAttenSlider->callback(cb_quadraticAttenSlides);

		// install slider min intensity
		m_thresholdSlider = new Fl_Value_Slider(10, 180, 200, 20, "Threshold");
		m_thresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_thresholdSlider->type(FL_HOR_NICE_SLIDER);
		m_thresholdSlider->labelfont(FL_COURIER);
		m_thresholdSlider->labelsize(12);
		m_thresholdSlider->minimum(0);
		m_thresholdSlider->maximum(1);
		m_thresholdSlider->step(0.01);
		m_thresholdSlider->value(0);
		m_thresholdSlider->align(FL_ALIGN_RIGHT);
		m_thresholdSlider->callback(cb_minIntensitySlides);

		// install slider super resolution
		m_antialiasingSlider = new Fl_Value_Slider(10, 205, 200, 20, "Antialiasing");
		m_antialiasingSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_antialiasingSlider->type(FL_HOR_NICE_SLIDER);
		m_antialiasingSlider->labelfont(FL_COURIER);
		m_antialiasingSlider->labelsize(12);
		m_antialiasingSlider->minimum(1);
		m_antialiasingSlider->maximum(5);
		m_antialiasingSlider->step(1);
		m_antialiasingSlider->value(1);
		m_antialiasingSlider->align(FL_ALIGN_RIGHT);
		m_antialiasingSlider->callback(cb_superResSlides);

		m_renderButton = new Fl_Button(260, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(260, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}