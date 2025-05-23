#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WBreak.h>
#include <Wt/WEnvironment.h>

class HelloFormApp : public Wt::WApplication {
public:
    HelloFormApp(const Wt::WEnvironment& env) : Wt::WApplication(env) {
        setTitle("Hello Form");

        root()->addWidget(std::make_unique<Wt::WText>("Enter your name: "));
        nameEdit_ = root()->addWidget(std::make_unique<Wt::WLineEdit>());
        root()->addWidget(std::make_unique<Wt::WBreak>());

        auto button = root()->addWidget(std::make_unique<Wt::WPushButton>("Submit"));
        result_ = root()->addWidget(std::make_unique<Wt::WText>());

        button->setMargin(5, Wt::Side::Top);

        button->clicked().connect(this, &HelloFormApp::onSubmit);
    }

private:
    std::unique_ptr<Wt::WLineEdit> nameEdit_;
    std::unique_ptr<Wt::WText> result_;

    void onSubmit() {
        auto name = nameEdit_->text().toUTF8();
        result_->setText("Hello, " + name + "!");
    }
};

Wt::WApplication* createApp(const Wt::WEnvironment& env) {
    return new HelloFormApp(env);
}

int main(int argc, char **argv) {
    return Wt::WRun(argc, argv, &createApp);
}
