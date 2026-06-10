#include "ofApp.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <GraphEditor.h>
#include <ofxImGuiUtils.h>

#define countof(array)						(sizeof(array)/sizeof((array)[0]))

GraphEditor::ViewState state;

ImRect ofToImRect(ofRectangle const& rect)
{
	return ImRect(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
}

ofRectangle ofToRect(ImRect const& rect)
{
	return ofRectangle(rect.Min.x, rect.Min.y, rect.Max.x - rect.Min.x, rect.Max.y - rect.Min.y);
}

class ofxLinker
{
public:
	template < class T >
	void link(ofParameter<T>& from, ofParameter<T>& to)
	{
		to = from;
		m_listener = from.newListener
		(
			[&](T& v)
			{
				to = v;
			}
		);
	}

	template < class T >
	void linkSerializable(ofParameter<T>& from, ofParameter<std::string>& to)
	{
		to = from.toString();
		m_listener = from.newListener
		(
			[&](T& v)
			{
				try 
				{
					to = ofToString<T>(v);
				}
				catch (...) 
				{
					ofLogError("link") << "Trying to serialize non-serializable parameter";
					to = "";
				}
			}
		);
	}

	void link(ofParameter<void>& from, ofParameter<void>& to)
	{
		m_listener = from.newListener
		(
			[&]()
			{
				to.trigger();
			}
		);
	}

	void delink()
	{
		m_listener.unsubscribe();
	}

private:
	ofEventListener m_listener;
};

namespace ofxImGui
{
	typedef bool(*LinkFunc)(ofxLinker&, ofAbstractParameter& from, ofAbstractParameter& to);
	typedef ofColor(*GetSlotColorFunc)(void);

	struct ValueType
	{
		std::string real;
		std::string alias;
		std::string category;
		GetSlotColorFunc	getSlotColorFunc = nullptr;
		LinkFunc			linkFunc = nullptr;
	};

	class ValueTypeManager
	{
	public:


	};

	typedef std::pair< std::string, ValueType > val_valType;
	typedef std::unordered_map< val_valType::first_type, val_valType::second_type > map_valType;

	map_valType g_map_valType;
	ValueType basic[] =
	{
		{
			typeid(void).name(),
			"event",
			"basic",
			[]() { return ofColor::lightPink; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					return false;
				}

				linker.link(from.cast<void>(), to.cast<void>());
				return true;
			}
		},
		{
			typeid(bool).name(),
			"",
			"basic",
			[]() { return ofColor::seaGreen; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<bool>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<bool>(), to.cast<bool>());
				return true;
			}
		},
		{
			typeid(int).name(),
			"",
			"basic",
			[]() { return ofColor::lightBlue; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<int>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<int>(), to.cast<int>());
				return true;
			}
		},
		{
			typeid(float).name(),
			"",
			"basic",
			[]() { return ofColor::red; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<float>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<float>(), to.cast<float>());
				return true;
			}
		},
		{
			typeid(std::string).name(),
			"string",
			"basic",
			[]() { return ofColor::lightPink; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					return false;
				}
				linker.link(from.cast<std::string>(), to.cast<std::string>());
				return true;
			}
		},
		{
			typeid(ofVec2f).name(),
			"vec2f",
			"basic",
			[]() { return ofColor::paleVioletRed; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<ofVec2f>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<ofVec2f>(), to.cast<ofVec2f>());
				return true;
			}
		},
		{
			typeid(ofVec3f).name(),
			"vec3f",
			"basic",
			[]() { return ofColor::mediumVioletRed; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<ofVec3f>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<ofVec3f>(), to.cast<ofVec3f>());
				return true;
			}
		},
		{
			typeid(ofVec4f).name(),
			"vec4f",
			"basic",
			[]() { return ofColor::mediumVioletRed; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<ofVec4f>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<ofVec4f>(), to.cast<ofVec4f>());
				return true;
			}
		},
		{
			typeid(ofColor).name(),
			"color",
			"basic",
			[]() { return ofColor::white; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<ofColor>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<ofColor>(), to.cast<ofColor>());
				return true;
			}
		},
		{
			typeid(ofRectangle).name(),
			"rect",
			"basic",
			[]() { return ofColor::orange; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<ofRectangle>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<ofRectangle>(), to.cast<ofRectangle>());
				return true;
			}
		},
		{
			typeid(ofQuaternion).name(),
			"quat",
			"basic",
			[]() { return ofColor::yellowGreen; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<ofQuaternion>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<ofQuaternion>(), to.cast<ofQuaternion>());
				return true;
			}
		},
		{
			typeid(ofMatrix3x3).name(),
			"mat33",
			"basic",
			[]() { return ofColor::lightGoldenRodYellow; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<ofMatrix3x3>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<ofMatrix3x3>(), to.cast<ofMatrix3x3>());
				return true;
			}
		},
		{
			typeid(ofMatrix4x4).name(),
			"mat44",
			"basic",
			[]() { return ofColor::lightGoldenRodYellow; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					if (to.valueType() == typeid(std::string).name())
					{
						linker.linkSerializable(from.cast<ofMatrix4x4>(), to.cast<std::string>());
						return true;
					}
					return false;
				}
				linker.link(from.cast<ofMatrix4x4>(), to.cast<ofMatrix4x4>());
				return true;
			}
		},
		{
			typeid(ofBaseDraws*).name(),
			"ofBaseDraws",
			"basic",
			[]() { return ofColor::turquoise; },
			[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
			{
				if (from.valueType() != to.valueType())
				{
					return false;
				}
				linker.link(from.cast<ofBaseDraws*>(), to.cast<ofBaseDraws*>());
				return true;
			}
		},
	};

	/*
	g_map_valType.insert
	(
		val_valType
		(
			typeid(int).name(),
			ValueType
			{
				"",
				ofColor::lightBlue,
				[](ofxLinker& linker, ofAbstractParameter& from, ofAbstractParameter& to)
				{
					linker.link(from.cast<int>(), to.cast<int>());
				}
			}
		)
	);
	*/


	bool gShowSlotType = true;

	ofColor typeToColor(std::string const& typeName)
	{
		map_valType::iterator iter = g_map_valType.find(typeName);
		if (iter == g_map_valType.end() || iter->second.getSlotColorFunc == nullptr)
		{
			return ofColor(128, 128, 128, 255);
		}

		return iter->second.getSlotColorFunc();
	}

	ImU32 ofToImCol32(ofColor const& color)
	{
		return IM_COL32(color.r, color.g, color.b, color.a);
	}

	class GraphTemplate;
	class GraphNode : public GraphEditor::Node
	{
	public:
		GraphNode()
		{
			m_param_node.setName("graph_node");
			m_param_node.add(m_param_alias.set("alias", ""));

			m_param_slot_in.setName("slot_in");
			m_param_slot_out.setName("slot_out");
		}

		virtual bool onSetup() = 0;
		virtual void onTemplateCreated(GraphTemplate*) {};
		virtual std::string onShowValueType(ofAbstractParameter const& param) 
		{
			map_valType::iterator iter = g_map_valType.find(param.valueType());
			if (iter == g_map_valType.end())
			{
				return param.valueType();
			}

			if (iter->second.alias.size() == 0)
			{
				return param.valueType();
			}
	
			return iter->second.alias;
		}
		
		virtual void onCustomDrawNode() {}
		virtual bool isDrawAlias() { return false; }
		virtual void onDelinkInput(GraphEditor::SlotIndex idx) {}
		virtual void onDelinkOutput(GraphEditor::SlotIndex idx) {}

		virtual bool needDraw() { return false; }
		virtual bool needUpdate() { return false; }
		virtual void draw() {}
		virtual void update() {}

		virtual bool setup() = 0;
		virtual void save(ofJson& json)
		{
			ofParameterGroup grp;
			grp.setName("basic");
			grp.add(ofParameter<std::string>().set("mName", mName));
			grp.add(ofParameter<ofRectangle>().set("mRect", m_rect));
			ofSerialize(json, grp);
			ofSerialize(json, m_param_node);
			ofSerialize(json, m_param_slot_in);
			ofSerialize(json, m_param_slot_out);
		}

		virtual void load(ofJson& json)
		{
			ofParameterGroup grp;
			ofParameter<ofRectangle> paramRect;
			grp.setName("basic");
			grp.add(ofParameter<std::string>().set("mName", mName));
			grp.add(paramRect.set("mRect", m_rect));

			m_rect = paramRect;

			ofDeserialize(json, grp);
			ofDeserialize(json, m_param_node);
			ofDeserialize(json, m_param_slot_in);
			ofDeserialize(json, m_param_slot_out);
		}

		std::string getAlias()
		{
			return m_param_alias;
		}

		void setAlias(std::string const& alias)
		{
			m_param_alias = alias;
		}

		void setPosition(float x, float y)
		{
			m_rect.setPosition(x, y);
			mRect = ofToImRect(m_rect);
		}

		void translate(float x, float y)
		{
			m_rect.translate(x, y);
			mRect = ofToImRect(m_rect);
		}

		ofVec2f getSize()
		{
			return ofVec2f(m_rect.width, m_rect.height);
		}

		void setSize(float w, float h)
		{
			m_rect.setSize(w, h);
			mRect = ofToImRect(m_rect);
		}

		void setSize(ofVec2f const& size)
		{
			m_rect.setSize(size.x, size.y);
			mRect = ofToImRect(m_rect);
		}

		ofAbstractParameter* getSlotInput(GraphEditor::SlotIndex index)
		{
			if (index >= m_param_slot_in.size())
			{
				return NULL;
			}

			return &m_param_slot_in[index];
		}

		ofAbstractParameter* getSlotOutput(GraphEditor::SlotIndex index)
		{
			if (index >= m_param_slot_out.size())
			{
				return NULL;
			}

			return &m_param_slot_out[index];
		}

	protected:
		ofParameter<std::string>	m_param_alias;
		ofParameterGroup			m_param_node;
		ofParameterGroup			m_param_slot_in;
		ofParameterGroup			m_param_slot_out;

		friend class GraphTemplateManager;

	private:
		ofRectangle					m_rect;
	};

	class GraphLink : public GraphEditor::Link
	{
	public:
		void setup
		(
			GraphEditor::NodeIndex inputNodeIndex,
			GraphEditor::SlotIndex inputSlotIndex,
			GraphEditor::NodeIndex outputNodeIndex,
			GraphEditor::SlotIndex outputSlotIndex
		)
		{
			mInputNodeIndex = inputNodeIndex;
			mInputSlotIndex = inputSlotIndex;
			mOutputNodeIndex = outputNodeIndex;
			mOutputSlotIndex = outputSlotIndex;
			//m_key = ofVAArgsToString("%d:%d:%d:%d", mInputNodeIndex, mInputNodeIndex, mOutputNodeIndex, mOutputSlotIndex);
		}

		/*
		std::string const& getKey()
		{
			return m_key;
		}
		*/

		bool link(ofAbstractParameter& from, ofAbstractParameter& to)
		{
			map_valType::iterator iter = g_map_valType.find(from.valueType());
			if (iter == g_map_valType.end() || iter->second.linkFunc == nullptr)
			{
				return false;
			}

			return iter->second.linkFunc(m_linker, from, to);
		}

		void delink()
		{
			m_linker.delink();
		}

	private:
		//std::string m_key;
		ofxLinker	m_linker;

	};

	class GraphTemplate : public GraphEditor::Template
	{
	public:
		virtual ~GraphTemplate()
		{
			if (m_buf_slot_in)
			{
				delete[] m_buf_slot_in;
			}

			if (mInputNames)
			{
				delete[] mInputNames;
			}

			if (mInputColors)
			{
				delete[] mInputColors;
			}

			if (m_buf_slot_out)
			{
				delete[] m_buf_slot_out;
			}

			if (mOutputNames)
			{
				delete[] mOutputNames;
			}

			if (mOutputColors)
			{
				delete[] mOutputColors;
			}
		}

		GraphEditor::TemplateIndex getIndex() const
		{
			return m_index;
		}

		std::string const& getKey() const
		{
			return m_key;
		}

		inline bool operator==(const GraphTemplate& rhs) { return (getKey().compare(rhs.getKey()) == 0); }
		inline bool operator!=(const GraphTemplate& rhs) { return !(*this == rhs); }

	private:
		std::string					m_key;
		GraphEditor::TemplateIndex	m_index;

		char* m_buf_slot_in;
		char* m_buf_slot_out;

		GraphTemplate(std::string const& key)
		: m_key(key)
		, m_index(0)
		, m_buf_slot_in(NULL)
		, m_buf_slot_out(NULL)
		{
			GraphEditor::Template* p_super = this;
			memset(p_super, 0x00, sizeof(GraphEditor::Template));

			mHeaderColor			= IM_COL32(203, 203, 203, 255);
			mHeaderTextColor		= IM_COL32(16,	16,	 16,  255);
			mBackgroundColor		= IM_COL32(128, 128, 128, 255);
			mBackgroundColorOver	= IM_COL32(128, 128, 128, 255);
		}

		void mf_setup(	GraphNode* pNode,
						ofParameterGroup const& slot_in, 
						ofParameterGroup const& slot_out)
		{
			mf_setupSlot(pNode, mInputCount, mInputNames, mInputColors, m_buf_slot_in, slot_in);
			mf_setupSlot(pNode, mOutputCount, mOutputNames, mOutputColors, m_buf_slot_out, slot_out);
		}

		void mf_setupSlot
		(
			GraphNode* pNode,
			ImU8& count,
			const char**& names,
			ImU32*& colors,
			char*& buf,
			ofParameterGroup const& slot
		)
		{
			size_t buf_size = 0;

			count = static_cast<ImU8>(slot.size());
			if (count == 0)
			{
				return;
			}

			names = new char const*[count];
			colors = new ImU32[count];

			//(valueType) name
			for (ImU8 i = 0; i < count; ++i)
			{
				ofAbstractParameter const& p = slot[i];
				if (gShowSlotType)
				{
					buf_size += (p.valueType().size() + 3);
				}

				buf_size += (p.getName().size() + 1);
			}

			buf = new char[buf_size];
			char* pch = buf;
			size_t buf_size_cur = buf_size;

			for (ImU8 i = 0; i < count; ++i)
			{
				ofAbstractParameter const& p = slot[i];
				size_t size_cpy = 0;
				if (gShowSlotType)
				{
					snprintf(pch, buf_size_cur, "(%s) %s", pNode->onShowValueType(p).c_str(), p.getName().c_str());
					size_cpy = (pNode->onShowValueType(p).size() + 3) + (p.getName().size() + 1);
				}
				else
				{
					snprintf(pch, buf_size_cur, "%s", p.getName().c_str());
					size_cpy = p.getName().size() + 1;
				}

				names[i] = pch;

				pch += size_cpy;
				buf_size_cur -= size_cpy;
				colors[i] = ofToImCol32(typeToColor(p.valueType()));
			}
		}

		friend class GraphTemplateManager;
	};

	class GraphTemplateManager
	{
	public:
		typedef std::string key_temp;
		typedef GraphEditor::TemplateIndex idx_temp;

		static GraphTemplateManager* GetInstance()
		{
			static GraphTemplateManager g_instacne;
			return &g_instacne;
		}

		virtual ~GraphTemplateManager()
		{
			for each (val_temp val in m_map_temp)
			{
				delete val.second;
			}
			m_map_temp.clear();
		}

		GraphTemplate* getTemplate(idx_temp index)
		{
			map_idx::iterator i_idx = m_map_idx.find(index);
			if (i_idx == m_map_idx.end())
			{
				return NULL;
			}

			map_temp::iterator i_tmp = m_map_temp.find(i_idx->second);
			if (i_tmp == m_map_temp.end())
			{
				return NULL;
			}
			return i_tmp->second;
		}

		const size_t getTemplateCount() const
		{
			return m_map_temp.size();
		}

		template< class T >
		GraphTemplate* getTemplate(GraphNode* pNode, bool& bCreate)
		{
			key_temp key = typeid(T).name();
			map_temp::iterator iter = m_map_temp.find(key);
			if (iter != m_map_temp.end())
			{
				bCreate = false;
				return iter->second;
			}

			GraphTemplate* p_temp = new GraphTemplate(key);
			p_temp->mf_setup(pNode, pNode->m_param_slot_in, pNode->m_param_slot_out);
			p_temp->m_index = m_map_temp.size();
			m_map_temp.insert(val_temp(key, p_temp));
			m_map_idx.insert(val_idx(p_temp->m_index, key));
			bCreate = true;
			return p_temp;
		}


	private:
		typedef std::pair< key_temp, GraphTemplate* >	val_temp;
		typedef std::pair< idx_temp, key_temp >			val_idx;

		typedef std::unordered_map< val_temp::first_type, val_temp::second_type > map_temp;
		typedef std::unordered_map< val_idx::first_type, val_idx::second_type > map_idx;

		map_temp	m_map_temp;
		map_idx		m_map_idx;
	};

	template< class SELF >
	class GraphNodeBase : public GraphNode
	{
	public:
		bool setup() override
		{
			bool yes = onSetup();
			if (!yes)
			{
				return false;
			}

			GraphTemplateManager* p = GraphTemplateManager::GetInstance();
			bool bCreated;
			GraphTemplate* p_temp = p->getTemplate<SELF>(this, bCreated);
			if (bCreated)
			{
				 onTemplateCreated(p_temp);
			}
			mTemplateIndex = p_temp->getIndex();
			return true;
		}
	};


	class GraphLinker : public GraphEditor::Link
	{
	private:
		ofxLinker m_linker;
	};

	class TriggerNode : public GraphNodeBase< TriggerNode >
	{
	public:
		bool onSetup() override
		{
			m_trigger.setName("signal");
			mName = "trigger";
			m_param_slot_out.add(m_trigger);
			setSize(192.f, 64.f);
			return true;
		}

		void onCustomDrawNode() override
		{
			if (ImGui::Button("trigger"))
			{
				m_trigger.trigger();
			}
		}

		/*
		std::string onGetValueType(ofAbstractParameter const& param) override
		{
			if (param.valueType() == typeid(std::string).name())
			{
				return "any";
			}
			else
			{
				return GraphNode::onGetValueType(param);
			}
		}
		*/

	private:
		ofParameter< void > m_trigger;
	};

	class SlotNode : public GraphNodeBase< SlotNode >
	{
	public:
		bool onSetup() override
		{
			m_slot.setName("slot");
			mName = "slot";
			m_param_slot_in.add(m_slot);
			setSize(192.f, 64.f);

			m_slot.addListener(this, &SlotNode::onTriggered);

			return true;
		}

		void onCustomDrawNode() override
		{

		}

		void onTriggered()
		{
			ofLogNotice("SlotNode", "onTriggered()");
		}
	

		/*
		std::string onGetValueType(ofAbstractParameter const& param) override
		{
			if (param.valueType() == typeid(std::string).name())
			{
				return "any";
			}
			else
			{
				return GraphNode::onGetValueType(param);
			}
		}
		*/

	private:
		ofParameter< void > m_slot;
	};

	class ManyToOneNode : public GraphNodeBase< ManyToOneNode >
	{
	public:
		ManyToOneNode(int count)
		: m_count(count)
		{}

		ManyToOneNode()
		: ManyToOneNode(4)
		{}

		bool onSetup() override
		{
			if (m_count <= 0 || m_count > 32)
			{
				return false;
			}

			mName = "n-to-1";

			for (int i = 0; i < m_count; ++i)
			{
				ofParameter< void > slot;
				slot.setName(ofVAArgsToString("%d", i));
				slot.addListener(this, &ManyToOneNode::onTriggered);
				m_param_slot_in.add(slot);
			}

			m_signal.setName("signal");
			m_param_slot_out.add(m_signal);
			setSize(64.f, 32.f * m_count);
			return true;
		}

		void onTriggered()
		{
			m_signal.trigger();
		}

	private:
		int									m_count;
		ofParameter< void >					m_signal;
	};

	class TraceNode : public GraphNodeBase< TraceNode >
	{
	public:
		bool onSetup() override
		{
			m_param.setName("value");
			mName = "trace";
			m_param_slot_in.add(m_param);
			setSize(192.f, 64.f);
			return true;
		}

		void onCustomDrawNode() override
		{
			if (ImGui::Button("trace"))
			{
				ofLogNotice("TraceNode", m_param);
			}
		}

		std::string onShowValueType(ofAbstractParameter const& param) override
		{
			if (param.valueType() == typeid(std::string).name())
			{
				return "any";
			}
			else
			{
				return GraphNode::onShowValueType(param);
			}
		}

	private:
		ofParameter< std::string > m_param;
	};

	class TextureNode : public GraphNodeBase < TextureNode >
	{
	public:
		TextureNode(std::string const& path)
		: m_path(path)
		, m_param(nullptr)
		{}

		TextureNode()
		: TextureNode("")
		{}

		bool onSetup() override
		{
			m_path.setName("path");

			m_is_show_detail.set("show detail info", true);
			m_is_show_origin.set("show origin", false);
			m_param_node.add(m_is_show_detail);
			m_param_node.add(m_is_show_origin);

			m_param = &m_tex;
			m_param.setName("texture");

			mName = "texture";

			m_param_slot_in.add(m_path);
			m_param_slot_out.add(m_param);

			m_path.addListener(this, &TextureNode::mf_onPathChanged);

			setSize(256.f, 256.f);

			if (m_path->size() != 0)
			{
				std::string tmp = m_path;
				mf_onPathChanged(tmp);
			}

			return true;
		}

		void onCustomDrawNode() override
		{
			utils::draw_text_input(&m_path, nullptr);
			utils::draw_bool(&m_is_show_detail, nullptr);
			utils::draw_bool(&m_is_show_origin, nullptr);
			utils::draw_texture(m_tex, m_is_show_detail, m_is_show_origin);
		}

	private:
		ofParameter< std::string >	m_path;
		ofParameter< ofBaseDraws* > m_param;
		ofTexture					m_tex;
		ofParameter< bool >			m_is_show_detail;
		ofParameter< bool >			m_is_show_origin;

		void mf_onPathChanged(std::string& v)
		{
			if (m_tex.isAllocated())
			{
				m_tex.clear();
			}

			ofPixels pixels;
			bool loaded = ofLoadImage(pixels, v);
			if (loaded) 
			{
				m_tex.allocate(pixels.getWidth(), pixels.getHeight(), ofGetGLInternalFormat(pixels));
				m_tex.loadData(pixels);
			}			
		}
	};

	class DrawsNode: public GraphNodeBase < DrawsNode >
	{
	public:
		DrawsNode()
		:m_slot(nullptr)
		{}

		bool onSetup() override
		{
			m_order.set("order", 0, 0, 512);
			m_rect.setName("rect");
			m_slot.setName("ofBaseDraws");

			mName = "draws";

			m_param_slot_in.add(m_order);
			m_param_slot_in.add(m_rect);
			m_param_slot_in.add(m_slot);

			setSize(256, 64.f + 256.f);
			return true;
		}

		void onCustomDrawNode() override
		{
			utils::draw_int_drag_default(&m_order, nullptr);
			utils::draw_rect(&m_rect, "%.3f");
		}

		void onDelinkInput(GraphEditor::SlotIndex idx) override
		{
			if (m_param_slot_in[idx].valueType() == m_slot.valueType())
			{
				m_slot = nullptr;
			}
		}


		bool needDraw() override 
		{ 
			return true; 
		}

		void draw() override
		{
			if (!m_slot.get())
			{
				return;
			}

			m_slot.get()->draw(m_rect);
		}

		std::string onShowValueType(ofAbstractParameter const& param) override
		{
			if (param.valueType() == typeid(ofBaseDraws*).name())
			{
				return "ofBaseDraws";
			}
			else
			{
				return GraphNode::onShowValueType(param);
			}
		}

	private:
		ofParameter< int >			m_order;
		ofParameter< ofRectangle >	m_rect;
		ofParameter< ofBaseDraws* > m_slot;
	};

	template< class T, int MODE = 0>
	class BaseTypeNode : public GraphNodeBase < BaseTypeNode< T > >
	{
	public: 
		BaseTypeNode(T const& val)
		: m_arg(NULL)
		{
			m_param = val;
		}

		bool onSetup() override
		{
			m_param.setName("value");

			map_valType::iterator iter = g_map_valType.find(typeid(T).name());

			if (iter != g_map_valType.end() && iter->second.alias.size() > 0)
			{
				mName = iter->second.alias.c_str();
			}
			else
			{
				mName = typeid(T).name();
			}

			if (typeid(T).name() == typeid(bool).name())
			{
				//m_func_draw = std::bind((utils::draw_param_func)&utils::draw_bool, std::placeholders::_1, nullptr);
				m_func_draw = (utils::draw_param_func)&utils::draw_bool;
			}
			else if (typeid(T).name() == typeid(int).name() || typeid(T).name() == typeid(unsigned int).name())
			{
				//m_func_draw = std::bind((utils::draw_param_func)&utils::draw_int_slider_default, std::placeholders::_1, nullptr);
				m_func_draw = (utils::draw_param_func)&utils::draw_int_drag_default;
			}
			else if (typeid(T).name() == typeid(float).name())
			{
				//m_func_draw = std::bind((utils::draw_param_func)&utils::draw_float_slider_default, std::placeholders::_1, (void*)"%.3f");
				m_func_draw = (utils::draw_param_func)&utils::draw_float_drag_default;
				m_arg = "%.3f";
			}
			else if (typeid(T).name() == typeid(ofRectangle).name())
			{
				//m_func_draw = std::bind((utils::draw_param_func)&utils::draw_float_slider_default, std::placeholders::_1, (void*)"%.3f");
				m_func_draw = (utils::draw_param_func)&utils::draw_rect;
				m_arg = "%.3f";
			}
			else if (typeid(T).name() == typeid(std::string).name())
			{
				//m_func_draw = std::bind((utils::draw_param_func)&utils::draw_text_input, std::placeholders::_1, nullptr);
				m_func_draw = (utils::draw_param_func)&utils::draw_text_input;
			}
			else
			{

				return false;
			}

			m_param_slot_out.add(m_param);
			setSize(192.f, 64.f);
			return true;
		}

		void onTemplateCreated(GraphTemplate* p_temp) override
		{

		}

		void onCustomDrawNode() override
		{
			if (!m_func_draw)
			{
				return;
			}

			m_func_draw(&m_param, m_arg);
		}

	private:
		ofParameter< T >		m_param;
		//utils::DrawParamFunc1	m_func_draw;
		utils::draw_param_func	m_func_draw;
		void*					m_arg;
	};


	typedef shared_ptr< GraphNode >(*MakeFunc)(void);
	struct NodeType
	{
		std::string		name;
		std::string		category;
		MakeFunc		makeFunc;
	};

	NodeType basicNodeType[] =
	{

		{
			"bool",
			"basic",
			[]() 
			{ 
				shared_ptr< GraphNode > sp_node = make_shared< BaseTypeNode< bool > >(false);
				return sp_node;//make_shared< GraphBoolNode >(false);
			}
		},
		{
			"int",
			"basic",
			[]()
			{
				shared_ptr< GraphNode > sp_node = make_shared< BaseTypeNode<int> >(0);
				return sp_node;
			}
		},
		{
			"float",
			"basic",
			[]()
			{
				shared_ptr< GraphNode > sp_node = make_shared< BaseTypeNode<float> >(0.f);
				return sp_node;
			}
		},
		{
			"rect",
			"basic",
			[]()
			{
				shared_ptr< GraphNode > sp_node = make_shared< BaseTypeNode<ofRectangle> >(ofRectangle());
				return sp_node;
			}
		},
		{
			"string",
			"basic",
			[]()
			{
				shared_ptr< GraphNode > sp_node = make_shared< BaseTypeNode<std::string> >("");
				return sp_node;
			}
		}
	};

	class GraphSystem: public GraphEditor::Delegate
	{
	public:
		GraphSystem()
		{}

		void init()
		{

		}

		void save()
		{
			ofJson jroot;
			ofJson jnodes;
			for (size_t i = 0; i < box.size(); ++i)
			{
				ofJson jnode;
				box[i]->save(jnode);
				jnodes.push_back(jnode);
			}

			jroot["nodes"] = jnodes;
			ofSaveJson("ccccc.json", jroot);
			/*
			tmp.setName("dddd");
			grp.add(tmp);
			ofSerialize(json, grp);
			ofSaveJson("ccccc.json", json);
			*/
		}

		void load()
		{

		}

		void update()
		{
			for each (auto var  in list_updates)
			{
				var->update();
			}
		}

		void draw()
		{
			for each (auto var  in list_draws)
			{
				var->draw();
			}
		}

		template< class T, class T1 >
		bool addNode(T1 const& val)
		{
			shared_ptr< GraphNode > sp_node = make_shared<T>(val);
			if (sp_node == nullptr)
			{
				return false;
			}

			if (!sp_node->setup())
			{
				return false;
			}

			box.push_back(sp_node);
			if (sp_node->needDraw())
			{
				list_draws.push_back(sp_node);
			}

			if (sp_node->needUpdate())
			{
				list_updates.push_back(sp_node);
			}
			return true;
		}

		template< class T>
		bool addNode()
		{
			shared_ptr< GraphNode > sp_node = make_shared<T>();
			if (sp_node == nullptr)
			{
				return false;
			}

			if (!sp_node->setup())
			{
				return false;
			}

			box.push_back(sp_node);
			if (sp_node->needDraw())
			{
				list_draws.push_back(sp_node);
			}

			if (sp_node->needUpdate())
			{
				list_updates.push_back(sp_node);
			}
			return true;
		}

		//GraphEditor::Delegate override
		//----------------------------------------------------------------
		bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override
		{
			return true;
		}

		void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override
		{
			if (box[nodeIndex]->mSelected == selected)
			{
				return;
			}

			if (selected)
			{
				selected_nodes.push_back(box[nodeIndex]);
			}
			else
			{
				selected_nodes.remove(box[nodeIndex]);
			}

			box[nodeIndex]->mSelected = selected;
		}
		
		void MoveSelectedNodes(const ImVec2 delta) override
		{
			for each (auto var in selected_nodes)
			{
				var->translate(delta.x, delta.y);
			}
		}

		void ResizeSelectedNode(GraphEditor::NodeIndex nodeIndex, const ImVec2 delta) override
		{
			shared_ptr< GraphNode > sp_node = box[nodeIndex];
			if (!sp_node)
			{
				return;
			}
			sp_node->setSize(sp_node->getSize() + ofVec2f(delta));
		}

		void CloseSelectedNode(GraphEditor::NodeIndex nodeIndex) override
		{
			shared_ptr< GraphNode > sp_node = box[nodeIndex];
			if (!sp_node)
			{
				return;
			}

			for (vec_link::iterator iter = links.begin(); iter != links.end();)
			{
				shared_ptr< GraphLink > sp_link = *iter;
				if (sp_link->mInputNodeIndex == nodeIndex || sp_link->mOutputNodeIndex == nodeIndex)
				{
					shared_ptr< GraphNode > sp_nodeFrom = box[sp_link->mInputNodeIndex];
					sp_nodeFrom->onDelinkOutput(sp_link->mInputSlotIndex);

					shared_ptr< GraphNode > sp_nodeTo = box[sp_link->mOutputNodeIndex];
					sp_nodeTo->onDelinkInput(sp_link->mOutputSlotIndex);

					iter = links.erase(iter);
					continue;
				}

				if (sp_link->mInputNodeIndex > nodeIndex)
				{
					--(sp_link->mInputNodeIndex);
				}

				if (sp_link->mOutputNodeIndex > nodeIndex)
				{
					--(sp_link->mOutputNodeIndex);
				}

				++iter;
			}

			box.erase(box.begin() + nodeIndex);

			if (sp_node->needDraw())
			{
				list_draws.remove(sp_node);
			}

			if (sp_node->needUpdate())
			{
				list_updates.remove(sp_node);
			}

		}

		void AddLink(	GraphEditor::NodeIndex inputNodeIndex, 
						GraphEditor::SlotIndex inputSlotIndex, 
						GraphEditor::NodeIndex outputNodeIndex, 
						GraphEditor::SlotIndex outputSlotIndex) override
		{
			shared_ptr< GraphNode > sp_nodeFrom = box[inputNodeIndex];
			if (!sp_nodeFrom)
			{
				return;
			}

			ofAbstractParameter* p_paramFrom = sp_nodeFrom->getSlotOutput(inputSlotIndex);
			if (!p_paramFrom)
			{
				return;
			}

			shared_ptr< GraphNode > sp_nodeTo = box[outputNodeIndex];
			if (!sp_nodeTo)
			{
				return;
			}

			ofAbstractParameter* p_paramTo = sp_nodeTo->getSlotInput(outputSlotIndex);
			if (!p_paramTo)
			{
				return;
			}

			shared_ptr< GraphLink > sp = make_shared<GraphLink>();
			sp->setup(inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex);
			if (!sp->link(*p_paramFrom, *p_paramTo))
			{
				return;
			}

			//m_map_links.insert(val_link(sp->getKey(), sp));
			links.push_back(sp);
		}

		void DelLink(GraphEditor::LinkIndex linkIndex) override
		{
			shared_ptr< GraphLink > sp = links[linkIndex];
			sp->delink();

			shared_ptr< GraphNode > sp_nodeFrom = box[sp->mInputNodeIndex];
			sp_nodeFrom->onDelinkOutput(sp->mInputSlotIndex);

			shared_ptr< GraphNode > sp_nodeTo = box[sp->mOutputNodeIndex];
			sp_nodeTo->onDelinkInput(sp->mOutputSlotIndex);

			//m_map_links.erase(m_map_links.begin() + linkIndex);
			links.erase(links.begin() + linkIndex);
		}

		// user is responsible for clipping
		void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override
		{
			ImVec2 size = rectangle.GetSize();
			if (size.y < 1.f || size.x < 1.f)
			{
				return;
			}

			ImGuiWindowFlags const window_flags = ImGuiWindowFlags_HorizontalScrollbar ;
			shared_ptr< GraphNode > sp_node = box[nodeIndex];
			ImGui::SetCursorScreenPos(rectangle.Min);

			ImGui::BeginChild("", size, true, window_flags);
			
			ImGui::Indent(16.f);

			if (sp_node->isDrawAlias())
			{
				enum { MaxSizeBuf = 256 + 128 };
				char buf[MaxSizeBuf];
				strncpy(buf, sp_node->getAlias().c_str(), MaxSizeBuf);
				buf[MaxSizeBuf - 1] = '\0';
				if (ImGui::InputText("alias", buf, MaxSizeBuf))
				{
					sp_node->setAlias(buf);
				}
			}

			sp_node->onCustomDrawNode();
			ImGui::EndChild();
		}

		// use mouse position to open context menu
		// if nodeIndex != -1, right click happens on the specified node
		void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override
		{
			/*
			if (nodeIndex != -1)
			{
				if (ImGui::BeginPopupContextItem("AA"))
				{
					if (ImGui::MenuItem("add bool"))
					{

					}
					ImGui::EndPopup();
				}
			}
			*/
		}

		const size_t GetTemplateCount() override
		{
			return GraphTemplateManager::GetInstance()->getTemplateCount();
		}

		const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
		{
			GraphTemplate* p_temp = GraphTemplateManager::GetInstance()->getTemplate(index);
			if (!p_temp)
			{
				return GraphEditor::Template();
			}
			else
			{
				return *p_temp;
			}
		}

		const size_t GetNodeCount() override
		{
			return box.size();
		}

		const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
		{
			/*
			shared_ptr< GraphNode > sp_node = box[index];
			return GraphEditor::Node
			{
				sp_node->mName,
				sp_node->mTemplateIndex,
				sp_node->mRect,
				sp_node->mSelected
			};
			*/
			return *box[index];
		}

		const size_t GetLinkCount() override
		{
			return links.size();
			//return m_map_links.size();
		}

		const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override
		{
			//map_link::iterator iter = m_map_links.begin() + index;
			//return *iter->second;
			return *links[index];
		}

		typedef std::pair< std::string, shared_ptr<GraphLink> > val_link;
		typedef std::unordered_map< val_link::first_type, val_link::second_type > map_link;
		typedef std::vector< shared_ptr< GraphLink > > vec_link;

		std::vector< shared_ptr< GraphNode > > box;



		std::vector< shared_ptr< GraphLink > > links;

		std::list< shared_ptr< GraphNode > > selected_nodes;
		std::list< shared_ptr< GraphNode > > list_draws;
		std::list< shared_ptr< GraphNode > > list_updates;
	} ;

}

void ofApp::mf_test(int& v)
{

}

void ofApp::mf_triger()
{
	int c = 0;
}

void ofToParameter(ofParameterGroup& group, GraphEditor::Node const& node)
{
	group.clear();
	group.setName("GraphEditor::Node");
	group.add(ofParameter<std::string>().set("mName", node.mName));
	group.add(ofParameter<int>().set("mTemplateIndex", (int)node.mTemplateIndex));
	group.add(ofParameter<ofRectangle>().set("mRect", ofToRect(node.mRect)));
	group.add(ofParameter<bool>().set("mSelected", node.mSelected));
}

bool ofToNode(GraphEditor::Node& node, ofParameterGroup const& group)
{
	if (group.getName() != "GraphEditor::Node")
	{
		return false;
	}

	node.mName = group.getString("mName").get().c_str();
	/*
	ofParameterGroup group;
	group.setName("GraphEditor::Node");
	group.add(ofParameter<std::string>().set("mName", node.mName));
	group.add(ofParameter<int>().set("mTemplateIndex", node.mTemplateIndex));
	group.add(ofParameter<ofRectangle>().set("mRect", ofToRect(node.mRect)));
	group.add(ofParameter<bool>().set("mSelected", node.mSelected));
	*/
	return true;
}

ofxImGui::GraphSystem graphSystem;
GraphEditor::Options op;

typedef ofxImGui::BaseTypeNode< bool > GraphBoolNode;
typedef ofxImGui::BaseTypeNode< int > GraphIntNode;
typedef ofxImGui::BaseTypeNode< float > GraphFloatNode;
typedef ofxImGui::BaseTypeNode< std::string > GraphStringNode;

//--------------------------------------------------------------
void ofApp::setup()
{
	ofDisableArbTex();
	using namespace ofxImGui;

	for (int i = 0; i < countof(basic); ++i)
	{
		g_map_valType.insert(ofxImGui::val_valType(basic[i].real, basic[i]));
	}


	//shared_ptr< GraphIntNode > sp_node2 = make_shared<GraphIntNode>();
	graphSystem.addNode<GraphIntNode>(0);
	graphSystem.addNode<GraphFloatNode>(0.f);

	/*
	GraphIntNode intnode;
	intnode.setup();

	GraphFloatNode floatnode;
	floatnode.setup();
	floatnode.onCustomDraw();
	*/


    //ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofParameter< ofRectangle > tmp;
	ofJson json;
	/*
	ofParameterGroup grp;
	tmp = ofRectangle(10.f, 20.f, 100.f, 200.f);
	//grp.setName("abcd);
	tmp.setName("dddd");
	grp.add(tmp);
	ofSerialize(json, grp);
	ofSaveJson("ccccc.json", json);
	*/
	/*
	GraphEditor::Node node;
	node.mName = "Test";
	node.mTemplateIndex = 3;
	node.mRect = ofToImRect(ofRectangle(20.f, 10.f, 300.f, 100.f));

	ofParameterGroup group;
	ofToParameter(group, node);
	ofSerialize(json, group);
	ofSaveJson("ccccc.json", json);

	ofParameterGroup group2;
	GraphEditor::Node node2;
	group2.setName("GraphEditor::Node");
	ofJson json2 = ofLoadJson("ccccc.json");
	ofDeserialize(json2, group2);
	ofToNode(node2, group2);
	*/



	/*
	if (json.is_array())
	{
		for each (auto var in json)
		{
			int tmp = var;
			int c = 0;
		}
	}
	*/


	ofParameter< void > output;
	ofParameter< void > input;

	input.addListener(this, &ofApp::mf_triger);

	ofxLinker linker;
	linker.link(output, input);

	output.trigger();

	/*
	input = output;



	auto abc = [&](int&v) 
	{
		input = v; 
	};

	{
		auto tmp = output.newListener(abc);
	}
	*/
	//output = 40;

	//int in = input;



	m_background_color.setName("Background Color");
	m_background_color.set(ofColor(114, 144, 154));

	typedef std::shared_ptr < ofParameter<int> >			sp_param_int;
	typedef std::shared_ptr < ofParameter<bool> >			sp_param_bool;
	typedef std::shared_ptr < ofParameter<float> >			sp_param_float;
	typedef std::shared_ptr < ofParameter< ofxImGuiInt > >	sp_param_gui_int;
	typedef std::shared_ptr < ofParameter< ofxImGuiEnum > >	sp_param_enum;

	m_param_group.setName("RPI Camera Settings");
	m_param_group.add(*sp_param_int(new ofParameter<int>("Sharpness", 0, -100, 100)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("Contrast", 0, -100, 100)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("Brightness", 50, 0, 100)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("Saturation", 0, -100, 100)));
	m_param_group.add(*sp_param_bool(new ofParameter<bool>("IsAutoISO", true)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("ISO", 100, 100, 2000)));
	m_param_group.add(*sp_param_bool(new ofParameter<bool>("IsAutoShutter", true)));
	m_param_group.add(*sp_param_gui_int(new ofParameter< ofxImGuiInt >("ShutterSpeed_MicroSec", ofxImGuiInt(0, 10, 100))));
	m_param_group.add(*sp_param_int(new ofParameter<int>("DynRangeExp", 0, 0, 3)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("Ev Compensation", 0, -4, 4)));

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("Off");
		enum_val.content.push_back("Auto");
		enum_val.content.push_back("SunLight");
		enum_val.content.push_back("Cloudy");
		enum_val.content.push_back("Shade");
		enum_val.content.push_back("Tungsten");
		enum_val.content.push_back("Fluorescent");
		enum_val.content.push_back("Incandescent");
		enum_val.content.push_back("Flash");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("WhiteBalance", enum_val)));
	}

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("None");
		enum_val.content.push_back("Noise");
		enum_val.content.push_back("Emboss");
		enum_val.content.push_back("Negative");
		enum_val.content.push_back("Sketch");
		enum_val.content.push_back("OilPaint");
		enum_val.content.push_back("Hatch");
		enum_val.content.push_back("Gpen");
		enum_val.content.push_back("Antialias");
		enum_val.content.push_back("DeRing");
		enum_val.content.push_back("Solarize");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("ImageFilter", enum_val)));
	}

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("Off");
		enum_val.content.push_back("Auto");
		enum_val.content.push_back("Night");
		enum_val.content.push_back("BackLight");
		enum_val.content.push_back("SpotLight");
		enum_val.content.push_back("Sports");
		enum_val.content.push_back("Snow");
		enum_val.content.push_back("Beach");
		enum_val.content.push_back("LargeAperture");
		enum_val.content.push_back("SmallApperture");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("Exposure", enum_val)));
	}

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("Average");
		enum_val.content.push_back("Spot");
		enum_val.content.push_back("Matrix");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("MeteringMode", enum_val)));
	}

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("None");
		enum_val.content.push_back("Vertical");
		enum_val.content.push_back("Horizontal");
		enum_val.content.push_back("Both");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("Mirror", enum_val)));
	}

	ofxImGuiParameter::Initialize();

	m_imgui_parameter.setup("ofxImGuiParameter", ofRectangle(10, 10, 480, 640));
	m_imgui_parameter.bind(m_background_color);
	m_imgui_parameter.bind(m_param_group);
	m_imgui_parameter.load();

	ofAddListener(m_param_group.parameterChangedE(), this, &ofApp::mf_on_parameter_changed);
	ofAddListener(ofxImGuiParameter::GetOnDrawEvent(), this, &ofApp::mf_on_ImGui_draw);
}

void ofApp::exit()
{
	ofxImGuiParameter::Finalize();
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{   
	ofSetBackgroundColor(m_background_color.get());
	
	graphSystem.draw();
	
	ofxImGuiParameter::Draw();
	
}

void ofApp::mf_on_parameter_changed(ofAbstractParameter& e)
{

}

ofRectangle	m_pos_and_size;

void ofApp::mf_on_ImGui_draw()
{
	//You can test any ImGui Code here.
	ImGui::Begin("Graph Editor", NULL, 0);

	if (ImGui::Button("save"))
	{
		graphSystem.save();
	}

	ImGui::SameLine();
	if (ImGui::Button("load"))
	{
		graphSystem.load();
	}


	GraphEditor::Show(graphSystem, op, state, true);

	//ImGui::OpenPopup("ABCDE");

	if (ImGui::BeginPopupContextItem("ABCDE"))
	{
		if (ImGui::MenuItem("add bool"))
		{
			graphSystem.addNode<GraphBoolNode>(false);
		}

		if (ImGui::MenuItem("add int"))
		{
			graphSystem.addNode<GraphIntNode>(0);
		}

		if (ImGui::MenuItem("add float"))
		{
			graphSystem.addNode<GraphFloatNode>(0);
		}

		if (ImGui::MenuItem("add rect"))
		{
			graphSystem.addNode<ofxImGui::BaseTypeNode <ofRectangle> >(ofRectangle());
		}

		if (ImGui::MenuItem("add string"))
		{
			graphSystem.addNode<GraphStringNode>("");
		}

		if (ImGui::MenuItem("add trace"))
		{
			graphSystem.addNode<ofxImGui::TraceNode>();
		}

		if (ImGui::MenuItem("add trigger"))
		{
			graphSystem.addNode<ofxImGui::TriggerNode>();
		}


		if (ImGui::MenuItem("add slot"))
		{
			graphSystem.addNode<ofxImGui::SlotNode>();
		}

		if (ImGui::MenuItem("add n-to-1"))
		{
			graphSystem.addNode<ofxImGui::ManyToOneNode>();
		}


		if (ImGui::MenuItem("add draws"))
		{
			graphSystem.addNode<ofxImGui::DrawsNode>();
		}

		if (ImGui::MenuItem("add texture"))
		{
			graphSystem.addNode<ofxImGui::TextureNode>();
		}

		ImGui::EndPopup();
	}


	ImGui::End();
	//if (m_is_fitting_window)window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

	/*
	ImGui::Begin("Graph Editor", NULL, 0);


	ImGui::SetNextWindowPos(ImVec2(200.f, 200.f), ImGuiCond_Always);
	//ImGui::SetNextWindowSize(ImVec2(300.f, 300.f), ImGuiCond_Always);

	ImGui::Begin("Test", NULL, 0);
	ImGui::Button("Button");
	ImGui::End();

	*/
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}


void ofApp::mouseScrolled(float x, float y){

}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
	graphSystem.addNode<ofxImGui::TextureNode>(dragInfo.files[0]);
}