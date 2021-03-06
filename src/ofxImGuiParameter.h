﻿/*
The78ester
*/

#ifndef INCLUDE_OF_ADDONS_OFXIMGUIPARAMETER_H_
#define INCLUDE_OF_ADDONS_OFXIMGUIPARAMETER_H_

#include <ofMain.h>

typedef void(*gf_draw_func)(ofAbstractParameter*);

struct ofxImGuiParamInfo
{
	shared_ptr<ofAbstractParameter>		sp_param;
	gf_draw_func						func;
	size_t								arg;
	std::vector< ofxImGuiParamInfo* >	children;
	bool								enable_sl;
};

template < typename TYPE >
class ofxROPSetter
{
	typedef ofxROPSetter < TYPE > Self;

public:
	static void setName(ofReadOnlyParameter< TYPE, Self >& param, std::string const& name)
	{
		param.setName(name);
	}

	static void set(ofReadOnlyParameter< TYPE, Self >& param, TYPE val)
	{
		param = val;
	}
};

typedef ofReadOnlyParameter< int, ofxROPSetter< int > > ofxROParamInt;
typedef ofReadOnlyParameter< float, ofxROPSetter< float > > ofxROParamFloat;
typedef ofReadOnlyParameter< std::string, ofxROPSetter< std::string > > ofxROParamString;

class ofxImGuiParameter
{
public:
	typedef ofxImGuiParamInfo ParamInfo;
	typedef uintptr_t BindedID;

	static BindedID const InvalidBindedID ;
	static ofRectangle const DefaultPosAndSize;

	enum Style
	{
		StyleNone,
		StyleInputField,
		StyleSlider,
		StyleDrag,
	};

	template < bool >
	struct EventType
	{

	};

	struct EnumType
	{
		int							select;
		std::vector < std::string > content;
	};

	template< typename TYPE >
	struct ValueType
	{
		TYPE	value;
		TYPE	arg0; //(Slider)min or (InputField)step
		TYPE	arg1; //(Slider)max or (InputField)step fast
		Style	style;

		ValueType()
		: value(0)
		, arg0(0)
		, arg1(0)
		, style(StyleInputField)
		{}

		ValueType(TYPE v)
		: value(v)
		, arg0(0)
		, arg1(0)
		, style(StyleInputField)
		{}

		ValueType(TYPE v, TYPE a0, TYPE a1 = 0)
		: value(v)
		, arg0(a0)
		, arg1(a1)
		, style(StyleInputField)
		{}

		ValueType(TYPE v, TYPE a0, TYPE a1, Style s)
		: value(v)
		, arg0(a0)
		, arg1(a1)
		, style(s)
		{}
	};

	template < typename TYPE >
	struct LabelType
	{
		TYPE value;
	};

	static bool GetEnumTypeFormDirectory(EnumType* p_out, std::string const& in_path, std::string const& ext_filter);

	static void Initialize();
	static void Finalize();

	static void Draw();
	static ofEvent< void >& GetOnDrawEvent();

	ofxImGuiParameter();
	~ofxImGuiParameter();

	bool setup(std::string const& title = "ofxImGuiParameter", ofRectangle const& rect = DefaultPosAndSize, Style default_style = StyleSlider);
	bool setup(std::string const& title, Style default_style);

	bool is_setup();
	void exit();
	BindedID bind(ofAbstractParameter const& param, Style style = StyleNone);
	void unbind(BindedID bid);

	bool enable_save_and_load(BindedID bid, bool yes);

	void draw();
	bool save(std::string const& filepath = "");
	bool load(std::string const& filepath = "");

	void set_xml_filepath(std::string const& file);
	void set_help(std::string const& help);

	bool is_visible();
	bool is_locked_shortcut();
	bool is_enable_dialog();
	bool is_fitting_window();

	void set_visible(bool yes);
	void lock_shortcut(bool yes);
	void enable_dialog(bool yes);
	void set_fit_window(bool yes);
	ofEvent< void >& get_on_pre_draw_parameter_event();
	ofEvent< void >& get_on_post_draw_parameter_event();

	ofEvent< std::string const >& get_on_pre_save_event();
	ofEvent< std::string const >& get_on_pre_load_event();

	ofEvent< std::string const >& get_on_save_event();
	ofEvent< std::string const >& get_on_load_event();

private:

	static std::vector< ofxImGuiParameter* >	s_box;
	static ofMutex								s_mutex;
	static ofEvent< void >						s_event;

	static void sf_draw(ParamInfo* p_param_info);

	ofEvent< void >						m_pre_draw_event;
	ofEvent< void >						m_post_draw_event;
	ofEvent< std::string const >		m_on_pre_save_event;
	ofEvent< std::string const >		m_on_pre_load_event;
	ofEvent< std::string const >		m_on_save_event;
	ofEvent< std::string const >		m_on_load_event;

	ofMutex						m_mutex;
	gf_draw_func				m_default_draw_i_func;
	gf_draw_func				m_default_draw_f_func;
	
	std::string					m_title;
	std::string					m_xml_filepath;
	std::string					m_msg_of_dialog;
	std::string					m_title_of_dialog;
	std::string					m_help;

	ofRectangle					m_pos_and_size;
	std::vector< ParamInfo* >	m_parameters;
	size_t						m_show_dialog;
	bool						m_is_visible;	
	bool						m_is_setup;
	bool						m_is_locked_shortcut;
	bool						m_is_focused;
	bool						m_is_enable_dialog;
	bool						m_is_dialog_auto_gone;
	bool						m_is_fitting_window;

	bool mf_setup(std::string const& title, ofRectangle const& rect, Style default_style);
	void mf_exit();
	void mf_draw_dialog();

	BindedID mf_bind(ofAbstractParameter const& param, std::vector< ParamInfo* >& contanier, Style style);

	void mf_unbind(std::vector< ParamInfo* >& contanier);
	void mf_unbind(BindedID bid, std::vector< ParamInfo* >& contanier);

	bool mf_enable_save_and_load(std::vector< ParamInfo* >& contanier, bool yes);
	bool mf_enable_save_and_load(BindedID bid, std::vector< ParamInfo* >& contanier, bool yes);

	void mf_show_dialog(std::string const& tittle, std::string const& message);
	
	void mf_internal_save();
	void mf_internal_load();

	void mf_check_and_exe_general_shortcut(ofKeyEventArgs& arg);

	void mf_on_key_pressed(ofKeyEventArgs& arg);
	bool mf_is_force_to_use_shortcut();

	//void mf_on_key_released(ofKeyEventArgs& arg)
};

inline void ofxImGuiParameter::set_xml_filepath(std::string const& file)
{
	m_xml_filepath = file;
}

inline void ofxImGuiParameter::set_help(std::string const& help)
{
	m_help = help;
}

inline bool ofxImGuiParameter::is_visible()
{
	return m_is_visible;
}

inline bool ofxImGuiParameter::is_locked_shortcut()
{
	return m_is_locked_shortcut;
}

inline bool ofxImGuiParameter::is_enable_dialog()
{
	return m_is_enable_dialog;
}
inline bool ofxImGuiParameter::is_fitting_window()
{
	return m_is_fitting_window;
}

inline void ofxImGuiParameter::set_visible(bool yes)
{
	m_is_visible = yes;
}

inline void ofxImGuiParameter::lock_shortcut(bool yes)
{
	m_is_locked_shortcut = yes;
}

inline void ofxImGuiParameter::enable_dialog(bool yes)
{
	m_is_enable_dialog = yes;
}

inline void ofxImGuiParameter::set_fit_window(bool yes)
{
	m_is_fitting_window = yes;
}

inline ofEvent< void >& ofxImGuiParameter::get_on_pre_draw_parameter_event()
{
	return m_pre_draw_event;
}

inline ofEvent< void >& ofxImGuiParameter::get_on_post_draw_parameter_event()
{
	return m_post_draw_event;
}

inline ofEvent< std::string const >& ofxImGuiParameter::get_on_pre_save_event()
{
	return m_on_pre_save_event;
}

inline ofEvent< std::string const >& ofxImGuiParameter::get_on_pre_load_event()
{
	return m_on_pre_load_event;
}

inline ofEvent< std::string const >& ofxImGuiParameter::get_on_save_event()
{
	return m_on_save_event;
}

inline ofEvent< std::string const >& ofxImGuiParameter::get_on_load_event()
{
	return m_on_load_event;
}

typedef ofxImGuiParameter::ValueType< int >		ofxImGuiInt;
typedef ofxImGuiParameter::ValueType< float	>	ofxImGuiFloat;
typedef ofxImGuiParameter::EnumType				ofxImGuiEnum;
typedef	ofxImGuiParameter::EventType< false >	ofxImGuiButton;
typedef ofxImGuiParameter::EventType< true >	ofxImGuiButtonSL;

typedef ofxImGuiParameter::LabelType< int >			ofxImGuiLabelInt;
typedef ofxImGuiParameter::LabelType< float	>		ofxImGuiLabelFloat;
typedef ofxImGuiParameter::LabelType< std::string >	ofxImGuiLabel;

#endif//INCLUDE_OF_ADDONS_OFXIMGUIPARAMETER_H_
