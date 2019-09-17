
#include "../amalgamated/mcl_amalgamated.h"


class MyApplication : public TApplication
{
	TWinForm form1, form2;
	TWinButton button1;
	TWinEditBox editBox1;
	TWinLabel label1;
	TWinTextArea textArea1;
	TWinCheckBox checkBox1, checkBox2;
	TWinRadioButton radioButton1, radioButton2;
	TWinMenuBar menuBar;
	TWinMenu fileMenu, helpMenu;
	TWinMenuItem miExit, miAbout;

public:

	MyApplication()
	{

	}

	void onClose(TComponent* sender)
	{

	}

	void onAboutWindowClose(TComponent* sender)
	{
		form2.exitModalLoop = true;
	}

	void button1Press(TComponent* sender)
	{
		TOpenDialog openDialog;
		openDialog.title = "Open text file...";
		openDialog.filter = "Text Files (*.txt)|*.txt";

		if (openDialog.show(form1))
		{
			TFile txtFile;
			if(txtFile.openFile(openDialog.fileName))
				textArea1.text = txtFile.readAsString();
		}
	}

	void editBoxChange(TComponent* sender)
	{
		label1.text = editBox1.text;
	}

	void checkBox1Press(TComponent* sender)
	{
		textArea1.wordWrap = checkBox1.value;
	}

	void checkBox2Press(TComponent* sender)
	{
		editBox1.readOnly = checkBox2.value;
	}

	void radioButtonPress(TComponent* sender)
	{
		if (sender == &radioButton1)
		{
			radioButton1.value = true;
			radioButton2.value = false;
			editBox1.lowercaseOnly = true;	
		}
		else
		{
			radioButton2.value = true;
			radioButton1.value = false;
			editBox1.uppercaseOnly = true;
		}
	}

	void onMenuItemExitPress(TMenuItem* sender)
	{
		form1.close();
	}

	void onMenuItemAboutPress(TMenuItem* sender)
	{
		form2.centerScreen();
		form2.showAsModal();
	}

	int main(TString** argv, int argc)
	{
		form1.text = "MCL Example";
		form1.onClose = EVENT(this, MyApplication::onClose);
		form1.maximizeButton = false;
		form1.width = 400;
		form1.height = 300;
		form1.centerScreen();

		form2.text = "About";
		form2.formType = TWinForm::FormTypes::TDIALOG;
		form2.closeBehaviour = TWinForm::CloseBehaviour::DO_NOTHING;
		form2.onClose = EVENT(this, MyApplication::onAboutWindowClose);
		form2.parent = form1;
		form2.width = 400;
		form2.height = 200;

		editBox1.left = 10;
		editBox1.top = 10;
		editBox1.parent = form1;
		editBox1.onChange = EVENT(this, MyApplication::editBoxChange);

		label1.text = "Label 1";
		label1.left = 150;
		label1.top = 10;
		label1.parent = form1;

		button1.text = "Browse";
		button1.left = 10;
		button1.top = 50;
		button1.parent = form1;
		button1.onPress = EVENT(this, MyApplication::button1Press);

		textArea1.left = 10;
		textArea1.top = 100;
		textArea1.parent = form1;

		checkBox1.text = "Word Wrap";
		checkBox1.left = 135;
		checkBox1.top = 55;
		checkBox1.parent = form1;
		checkBox1.onCheck = EVENT(this, MyApplication::checkBox1Press);

		checkBox2.text = "Read Only";
		checkBox2.left = 258;
		checkBox2.top = 5;
		checkBox2.parent = form1;
		checkBox2.onCheck = EVENT(this, MyApplication::checkBox2Press);

		radioButton1.text = "Small Case";
		radioButton1.value = true;
		radioButton1.left = 258;
		radioButton1.top = 30;
		radioButton1.parent = form1;
		radioButton1.onCheck = EVENT(this, MyApplication::radioButtonPress);

		radioButton2.text = "Upper Case";
		radioButton2.left = 258;
		radioButton2.top = 50;
		radioButton2.parent = form1;
		radioButton2.onCheck = EVENT(this, MyApplication::radioButtonPress);

		miExit.text = "Exit";
		miExit.onPress = EVENT(this, MyApplication::onMenuItemExitPress);

		miAbout.text = "About"; 
		miAbout.onPress = EVENT(this, MyApplication::onMenuItemAboutPress);

		fileMenu.add(miExit);
		helpMenu.add(miAbout);

		menuBar.add("File", fileMenu);
		menuBar.add("Help", helpMenu);

		form1.menu = menuBar;
		form1.visible = true;

		TApplication::run(); // message loop (this function will not return until you call TApplication::shouldQuit method.)

		return 0;
	}
};

START_APPLICATION(MyApplication);