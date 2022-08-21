#include "MainComponent.h"

#define DECLARE_TYPE(ID) namespace TYPEs { const juce::Identifier ID(#ID); }
#define DECLARE_PROP(ID) namespace PROPs { const juce::Identifier ID(#ID); }
DECLARE_TYPE(root)
DECLARE_TYPE(textButton)
DECLARE_PROP(name)
DECLARE_PROP(x)
DECLARE_PROP(y)
DECLARE_PROP(w)
DECLARE_PROP(h)
DECLARE_PROP(text)
#undef DECLARE_TYPE
#undef DECLARE_PROP


//==============================================================================
// ComponentBuilderによってComponentTypeクラスを作成するためのクラス
template<typename ComponentType>
class CustomTypeHandler : public juce::ComponentBuilder::TypeHandler
{
public:
    CustomTypeHandler (const juce::Identifier &valueTreeType) : TypeHandler(valueTreeType) {}
    
    // ValueTreeを元にcomponentを作成し, 親コンポーネントに追加後そのcomponentを返す
    juce::Component* addNewComponentFromState (const juce::ValueTree &state, juce::Component *parent) override
    {
        juce::Component* comp = new ComponentType();
        if (parent) parent->addAndMakeVisible(comp);
        
        // 更新処理
        updateComponentFromState(comp, state);

        return comp;
    }
    
    // ValueTreeを元にcomponentの更新処理
    void updateComponentFromState (juce::Component *component, const juce::ValueTree &state) override
    {
        // 以下のプロパティが定義されている前提
        jassert(state.hasProperty(juce::ComponentBuilder::idProperty));
        jassert(state.hasProperty(PROPs::name));
        jassert(state.hasProperty(PROPs::x));
        jassert(state.hasProperty(PROPs::y));
        jassert(state.hasProperty(PROPs::w));
        jassert(state.hasProperty(PROPs::h));

        // プロパティを元にGUIコンポーネントの更新処理
        component->setName(state[PROPs::name]);
        component->setBounds(state[PROPs::x], state[PROPs::y], state[PROPs::w], state[PROPs::h]);
        
        // タイプIDに合わせた更新処理
        if (state.hasType(TYPEs::textButton) && state.hasProperty(PROPs::text))
        {
            if (auto* textButton = dynamic_cast<juce::TextButton*>(component))
                textButton->setButtonText(state[PROPs::text].toString());
        }
        
        // ValueTree子要素に対応したコンポーネントを作成
        getBuilder()->updateChildComponents(*component, state);
    }
};
//==============================================================================
MainComponent::MainComponent() : vtComponentBuilderRoot(TYPEs::root)
{
    // componentBuilderでの対象ValueTreeの初期化処理    
    initializeValueTree();
    
    // componentBuilderの初期化処理
    componentBuilder.reset(new juce::ComponentBuilder(vtComponentBuilderRoot));
    componentBuilder->registerTypeHandler(new CustomTypeHandler<juce::Component>(TYPEs::root));
    componentBuilder->registerTypeHandler(new CustomTypeHandler<juce::TextButton>(TYPEs::textButton));
    
    // 関数内に何も処理が記述されていない
//    componentBuilder->registerStandardComponentTypes();
    
    // 初回getManagedComponent()時にValueTreeとTypeHandlerを用いてルートComponentが生成され、取得できる。
    // このComponentの所有権はComponentBuilder側にあるため使用側でdeleteしないよう注意
    builderRootComponent = componentBuilder->getManagedComponent();
    addAndMakeVisible(builderRootComponent);
    
    
    // addButton
    addButton.reset(new juce::TextButton("add"));
    addAndMakeVisible(addButton.get());
    addButton->setBounds(0, 0, 100, 32);
    addButton->onClick = [this] { addToValueTree(); };
    
    // removeButton
    removeButton.reset(new juce::TextButton("remove"));
    addAndMakeVisible(removeButton.get());
    removeButton->setBounds(0, 32, 100, 32);
    removeButton->onClick = [this] { removeFromValueTree(); };
    
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
    addButton = nullptr;
    removeButton = nullptr;
    
    componentBuilder = nullptr;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{

}

//==============================================================================
void MainComponent::initializeValueTree()
{
    // ルートコンポーネントとなるValueTreeの用意
    vtComponentBuilderRoot.setProperty(juce::ComponentBuilder::idProperty, "root", nullptr);
    vtComponentBuilderRoot.setProperty(PROPs::name, "root", nullptr);
    vtComponentBuilderRoot.setProperty(PROPs::x, 100, nullptr);
    vtComponentBuilderRoot.setProperty(PROPs::y, 0, nullptr);
    vtComponentBuilderRoot.setProperty(PROPs::w, 500, nullptr);
    vtComponentBuilderRoot.setProperty(PROPs::h, 400, nullptr);
}

//==============================================================================
void MainComponent::addToValueTree()
{
    auto vtTextButton = juce::ValueTree(TYPEs::textButton);
    vtTextButton.setProperty(juce::ComponentBuilder::idProperty,
                             "textButton" + juce::String(vtComponentBuilderRoot.getNumChildren()), nullptr);
    vtTextButton.setProperty(PROPs::name, "textButton", nullptr);
    vtTextButton.setProperty(PROPs::x, juce::Random().nextInt({0, 400}), nullptr);
    vtTextButton.setProperty(PROPs::y, juce::Random().nextInt(300), nullptr);
    vtTextButton.setProperty(PROPs::w, 100, nullptr);
    vtTextButton.setProperty(PROPs::h, 100, nullptr);
    vtTextButton.setProperty(PROPs::text, vtTextButton[juce::ComponentBuilder::idProperty].toString(), nullptr);
    vtComponentBuilderRoot.appendChild(vtTextButton, nullptr);
}

//==============================================================================
void MainComponent::removeFromValueTree()
{
    auto numChildren = vtComponentBuilderRoot.getNumChildren();
    if (numChildren > 0)
    {
        vtComponentBuilderRoot.removeChild(numChildren - 1, nullptr);
    }
}
