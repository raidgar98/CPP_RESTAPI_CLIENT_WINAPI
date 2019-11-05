#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class AddHostnameDialog : public DialogWindow::LaunchOptions
{

	std::unique_ptr<DialogWindow> p{ nullptr };

	Label l1, l2;
	TextEditor hostname, address;
	
public:
	
	std::wstring hn{ L"" }, add{ L"" };

	AddHostnameDialog()
	{
		p.reset(create());
		p->setSize(250, 150);

		l1.setText(L"Nazwa Hosta:", NotificationType::dontSendNotification);
		l1.setBounds(10, 10, 100, 25);
		hostname.setBounds(125, 10, 110, 25);

		l2.setText(L"Adres IP:", NotificationType::dontSendNotification);
		l2.setBounds(10, 50, 100, 25);
		address.setBounds(125, 50, 110, 25);

		p->addAndMakeVisible(l1);
		p->addAndMakeVisible(l2);
		p->addAndMakeVisible(hostname);
		p->addAndMakeVisible(address);
		
		p->enterModalState(true, nullptr, false);

		int res = p->runModalLoop();
		const auto s1 = hostname.getText().toStdString();
		const auto s2 = address.getText().toStdString();

		for (const char var : s1)
			hn += var;

		for (const char var : s2)
			add += var;
	}

};