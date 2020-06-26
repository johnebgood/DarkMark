/* DarkMark (C) 2019-2020 Stephane Charette <stephanecharette@gmail.com>
 * $Id: StartupWnd.cpp 2989 2020-06-13 02:10:17Z stephane $
 */

#include "DarkMark.hpp"
#include <random>


dm::VideoImportWindow::VideoImportWindow(const std::string & dir, const VStr & v) :
	DocumentWindow("DarkMark - Import Video Frames", Colours::darkgrey, TitleBarButtons::allButtons),
	ThreadWithProgressWindow("DarkMark", true, true							),
	base_directory			(dir											),
	filenames				(v												),
	tb_extract_all			("extract all frames"							),
	tb_extract_maximum		("maximum number of random frames to extract:"	),
	sl_maximum				(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxRight),
	tb_extract_percentage	("percentage of random frames to extract:"		),
	sl_percentage			(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxRight),
	tb_do_not_resize		("do not resize frames"							),
	tb_resize				("resize frames:"								),
	txt_x					("", "x"										),
	tb_keep_aspect_ratio	("maintain aspect ratio"						),
	tb_force_resize			("resize to exact dimensions"					),
	tb_save_as_png			("save as PNG"									),
	tb_save_as_jpeg			("save as JPEG"									),
	txt_jpeg_quality		("", "image quality:"							),
	sl_jpeg_quality			(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxRight),
	cancel					("Cancel"),
	ok						("Start..."),
	number_of_processed_frames(0)
{
	setContentNonOwned		(&canvas, true	);
	setUsingNativeTitleBar	(true			);
	setResizable			(true, false	);
	setDropShadowEnabled	(true			);

	canvas.addAndMakeVisible(header_message			);
	canvas.addAndMakeVisible(tb_extract_all			);
	canvas.addAndMakeVisible(tb_extract_maximum		);
	canvas.addAndMakeVisible(sl_maximum				);
	canvas.addAndMakeVisible(tb_extract_percentage	);
	canvas.addAndMakeVisible(sl_percentage			);
	canvas.addAndMakeVisible(tb_do_not_resize		);
	canvas.addAndMakeVisible(tb_resize				);
	canvas.addAndMakeVisible(ef_width				);
	canvas.addAndMakeVisible(txt_x					);
	canvas.addAndMakeVisible(ef_height				);
	canvas.addAndMakeVisible(tb_keep_aspect_ratio	);
	canvas.addAndMakeVisible(tb_force_resize		);
	canvas.addAndMakeVisible(tb_save_as_png			);
	canvas.addAndMakeVisible(tb_save_as_jpeg		);
	canvas.addAndMakeVisible(txt_jpeg_quality		);
	canvas.addAndMakeVisible(sl_jpeg_quality		);
	canvas.addAndMakeVisible(cancel					);
	canvas.addAndMakeVisible(ok						);

	tb_extract_all			.setRadioGroupId(1);
	tb_extract_maximum		.setRadioGroupId(1);
	tb_extract_percentage	.setRadioGroupId(1);

	tb_do_not_resize		.setRadioGroupId(2);
	tb_resize				.setRadioGroupId(2);

	tb_keep_aspect_ratio	.setRadioGroupId(3);
	tb_force_resize			.setRadioGroupId(3);

	tb_save_as_png			.setRadioGroupId(4);
	tb_save_as_jpeg			.setRadioGroupId(4);

	tb_extract_all			.addListener(this);
	tb_extract_maximum		.addListener(this);
	tb_extract_percentage	.addListener(this);
	tb_do_not_resize		.addListener(this);
	tb_resize				.addListener(this);
	tb_keep_aspect_ratio	.addListener(this);
	tb_force_resize			.addListener(this);
	tb_save_as_png			.addListener(this);
	tb_save_as_jpeg			.addListener(this);
	cancel					.addListener(this);
	ok						.addListener(this);

	tb_extract_all			.setToggleState(true, NotificationType::sendNotification);
	tb_do_not_resize		.setToggleState(true, NotificationType::sendNotification);
	tb_keep_aspect_ratio	.setToggleState(true, NotificationType::sendNotification);
	tb_save_as_jpeg			.setToggleState(true, NotificationType::sendNotification);

	sl_maximum				.setRange(1.0, 9999.0, 1.0);
	sl_maximum				.setNumDecimalPlacesToDisplay(0);
	sl_maximum				.setValue(500.0);

	sl_percentage			.setRange(1.0, 99.0, 1.0);
	sl_percentage			.setNumDecimalPlacesToDisplay(0);
	sl_percentage			.setValue(25.0);

	sl_jpeg_quality			.setRange(30.0, 99.0, 1.0);
	sl_jpeg_quality			.setNumDecimalPlacesToDisplay(0);
	sl_jpeg_quality			.setValue(75.0);

	ef_width				.setInputRestrictions(5, "0123456789");
	ef_height				.setInputRestrictions(5, "0123456789");
	ef_width				.setText("640");
	ef_height				.setText("480");
	ef_width				.setJustification(Justification::centred);
	ef_height				.setJustification(Justification::centred);

	std::stringstream ss;
	try
	{
		const std::string filename = filenames.at(0);
		const std::string shortname = File(filename).getFileName().toStdString();

		cv::VideoCapture cap;
		cap.open(filename);
		const auto number_of_frames	= cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT	);
		const auto fps				= cap.get(cv::VideoCaptureProperties::CAP_PROP_FPS			);
		const auto frame_width		= cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH	);
		const auto frame_height		= cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT	);

		const size_t len_minutes = static_cast<size_t>(static_cast<size_t>(number_of_frames / fps) / 60);
		const size_t len_seconds = static_cast<size_t>(static_cast<size_t>(number_of_frames / fps) % 60);

		if (v.size() > 1)
		{
			ss << "There are " << v.size() << " videos to import." << std::endl << std::endl;
		}

		ss	<< "The video file \"" << shortname << "\" contains " << number_of_frames << " frames at " << fps << " FPS "
			<< "for a total length of " << len_minutes << "m " << len_seconds << "s. " << std::endl
			<< std::endl
			<< "Each frame is " << static_cast<int>(frame_width) << " x " << static_cast<int>(frame_height) << ".";
	}
	catch (...)
	{
		ss << "Error reading video file \"" << filenames.at(0) << "\".";
	}
	header_message.setText(ss.str(), NotificationType::sendNotification);

	setIcon(DarkMarkLogo());
	ComponentPeer *peer = getPeer();
	if (peer)
	{
		peer->setIcon(DarkMarkLogo());
	}

	if (cfg().containsKey("VideoImportWnd"))
	{
		restoreWindowStateFromString(cfg().getValue("VideoImportWnd"));
	}
	else
	{
		centreWithSize(400, 600);
	}

	setVisible(true);

	return;
}


dm::VideoImportWindow::~VideoImportWindow()
{
	signalThreadShouldExit();
	cfg().setValue("VideoImportWnd", getWindowStateAsString());

	return;
}


void dm::VideoImportWindow::closeButtonPressed()
{
	// close button

	setVisible(false);
	exitModalState(0);

	return;
}


void dm::VideoImportWindow::userTriedToCloseWindow()
{
	// ALT+F4

	closeButtonPressed();

	return;
}


void dm::VideoImportWindow::resized()
{
	// get the document window to resize the canvas, then we'll deal with the rest of the components
	DocumentWindow::resized();

	const auto height = 20.0f;
	const auto margin_size = 5;
	const FlexItem::Margin left_indent(0.0f, 0.0f, 0.0f, margin_size * 5.0f);
	const FlexItem::Margin new_row_indent(margin_size * 5.0f, 0.0f, 0.0f, 0.0f);

	FlexBox fb_rows;
	fb_rows.flexDirection	= FlexBox::Direction::column;
	fb_rows.alignItems		= FlexBox::AlignItems::stretch;
	fb_rows.justifyContent	= FlexBox::JustifyContent::flexStart;

	fb_rows.items.add(FlexItem(header_message			).withHeight(height * 4));
	fb_rows.items.add(FlexItem(tb_extract_all			).withHeight(height).withMargin(new_row_indent));
	fb_rows.items.add(FlexItem(tb_extract_maximum		).withHeight(height));
	fb_rows.items.add(FlexItem(sl_maximum				).withHeight(height).withWidth(150.0f).withMargin(left_indent));
	fb_rows.items.add(FlexItem(tb_extract_percentage	).withHeight(height));
	fb_rows.items.add(FlexItem(sl_percentage			).withHeight(height).withWidth(150.0f).withMargin(left_indent));
	fb_rows.items.add(FlexItem(tb_do_not_resize			).withHeight(height).withMargin(new_row_indent));
	fb_rows.items.add(FlexItem(tb_resize				).withHeight(height));

	FlexBox fb_dimensions;
	fb_dimensions.flexDirection	= FlexBox::Direction::row;
	fb_dimensions.justifyContent = FlexBox::JustifyContent::flexStart;
	fb_dimensions.items.add(FlexItem(ef_width			).withHeight(height).withWidth(50.0f));
	fb_dimensions.items.add(FlexItem(txt_x				).withHeight(height).withWidth(20.0f));
	fb_dimensions.items.add(FlexItem(ef_height			).withHeight(height).withWidth(50.0f));
	fb_rows.items.add(FlexItem(fb_dimensions			).withHeight(height).withMargin(left_indent));

	fb_rows.items.add(FlexItem(tb_keep_aspect_ratio		).withHeight(height).withMargin(left_indent));
	fb_rows.items.add(FlexItem(tb_force_resize			).withHeight(height).withMargin(left_indent));
	fb_rows.items.add(FlexItem(tb_save_as_png			).withHeight(height).withMargin(new_row_indent));
	fb_rows.items.add(FlexItem(tb_save_as_jpeg			).withHeight(height));

	FlexBox fb_quality;
	fb_quality.flexDirection = FlexBox::Direction::row;
	fb_quality.justifyContent = FlexBox::JustifyContent::flexStart;
	fb_quality.items.add(FlexItem(txt_jpeg_quality		).withHeight(height).withWidth(100.0f));
	fb_quality.items.add(FlexItem(sl_jpeg_quality		).withHeight(height).withWidth(150.0f));
	fb_rows.items.add(FlexItem(fb_quality				).withHeight(height).withMargin(left_indent));

	FlexBox button_row;
	button_row.flexDirection = FlexBox::Direction::row;
	button_row.justifyContent = FlexBox::JustifyContent::flexEnd;
	button_row.items.add(FlexItem()				.withFlex(1.0));
	button_row.items.add(FlexItem(cancel)		.withWidth(100.0).withMargin(FlexItem::Margin(0, margin_size, 0, 0)));
	button_row.items.add(FlexItem(ok)			.withWidth(100.0));

	fb_rows.items.add(FlexItem().withFlex(1.0));
	fb_rows.items.add(FlexItem(button_row).withHeight(30.0));
	
	auto r = getLocalBounds();
	r.reduce(margin_size, margin_size);
	fb_rows.performLayout(r);

	return;
}


void dm::VideoImportWindow::buttonClicked(Button * button)
{
	if (button == &cancel)
	{
		closeButtonPressed();
	}

	bool b = tb_extract_maximum.getToggleState();
	sl_maximum.setEnabled(b);

	b = tb_extract_percentage.getToggleState();
	sl_percentage.setEnabled(b);

	b = tb_resize.getToggleState();
	ef_width.setEnabled(b);
	txt_x.setEnabled(b);
	ef_height.setEnabled(b);
	tb_keep_aspect_ratio.setEnabled(b);
	tb_force_resize.setEnabled(b);

	b = tb_save_as_jpeg.getToggleState();
	txt_jpeg_quality.setEnabled(b);
	sl_jpeg_quality.setEnabled(b);

	if (button == &ok)
	{
		// disable all of the controls and start the fame extraction
		canvas.setEnabled(false);
		runThread(); // this waits for the thread to be done
		closeButtonPressed();
	}

	return;
}


void dm::VideoImportWindow::run()
{
	std::string current_filename		= "?";
	double work_completed				= 0.0;
	double work_to_be_done				= 1.0;
	bool error_shown					= false;
	number_of_processed_frames			= 0;

	try
	{
		const bool extract_all_frames		= tb_extract_all		.getToggleState();
		const bool extract_maximum_frames	= tb_extract_maximum	.getToggleState();
		const bool extract_percentage		= tb_extract_percentage	.getToggleState();
		const double maximum_to_extract		= sl_maximum			.getValue();
		const double percent_to_extract		= sl_percentage			.getValue() / 100.0;
		const bool resize_frame				= tb_resize				.getToggleState();
		const bool maintain_aspect_ratio	= tb_keep_aspect_ratio	.getToggleState();
		const int new_width					= std::atoi(ef_width	.getText().toStdString().c_str());
		const int new_height				= std::atoi(ef_height	.getText().toStdString().c_str());
		const bool save_as_png				= tb_save_as_png		.getToggleState();
		const bool save_as_jpg				= tb_save_as_jpeg		.getToggleState();
		const int jpg_quality				= sl_jpeg_quality		.getValue();

		setStatusMessage("Determining the amount of frames to extract...");

		for (auto && filename : filenames)
		{
			if (threadShouldExit())
			{
				break;
			}

			current_filename = filename;

			cv::VideoCapture cap;
			cap.open(filename);
			const auto number_of_frames = cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT);

			if (extract_all_frames)
			{
				work_to_be_done += number_of_frames;
			}
			else if (extract_maximum_frames)
			{
				work_to_be_done += std::min(number_of_frames, maximum_to_extract);
			}
			else if (extract_percentage)
			{
				work_to_be_done += number_of_frames * percent_to_extract;
			}
		}

		// now start actually extracting frames
		for (auto && filename : filenames)
		{
			if (threadShouldExit())
			{
				break;
			}

			current_filename = filename;

			const std::string shortname = File(filename).getFileName().toStdString(); // filename+extension, but no path

			File dir(base_directory);
			File child = dir.getChildFile(Time::getCurrentTime().formatted("video_import_%Y-%m-%d_%H-%M-%S"));
			child.createDirectory();
			const std::string partial_output_filename = child.getChildFile(shortname).getFullPathName().toStdString();

			setStatusMessage("Processing video file " + shortname + "...");

			cv::VideoCapture cap;
			cap.open(filename);
			const auto number_of_frames = cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT);

			std::random_device rd;
			std::mt19937 rng(rd());
			std::uniform_int_distribution<size_t> uni(0, number_of_frames - 1);

			std::set<size_t> frames_needed;
			if (extract_maximum_frames)
			{
				while (threadShouldExit() == false and frames_needed.size() < std::min(number_of_frames, maximum_to_extract))
				{
					const auto random_frame = uni(rng);
					frames_needed.insert(random_frame);
				}
			}
			else if (extract_percentage)
			{
				while (threadShouldExit() == false and frames_needed.size() < percent_to_extract * number_of_frames)
				{
					const auto random_frame = uni(rng);
					frames_needed.insert(random_frame);
				}
			}

			size_t frame_number = 0;
			while (threadShouldExit() == false)
			{
				if (extract_maximum_frames or extract_percentage)
				{
					if (frames_needed.empty())
					{
						// we've extracted all the frames we need
						break;
					}

					// we need to skip to a specific frame
					frame_number = *frames_needed.begin();
					frames_needed.erase(frame_number);
					cap.set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, static_cast<double>(frame_number));
				}

				cv::Mat mat;
				cap >> mat;
				if (mat.empty())
				{
					// must have reached the EOF
					break;
				}

				if (resize_frame and (mat.cols != new_width or mat.rows != new_height))
				{
					if (maintain_aspect_ratio)
					{
						mat = resize_keeping_aspect_ratio(mat, {new_width, new_height});
					}
					else
					{
						cv::Mat dst;
						cv::resize(mat, dst, {new_width, new_height}, 0, 0,  CV_INTER_AREA);
						mat = dst;
					}
				}

				std::stringstream ss;
				ss << partial_output_filename << "_frame_" << std::setfill('0') << std::setw(6) << frame_number;

				if (save_as_png)
				{
					cv::imwrite(ss.str() + ".png", mat, {CV_IMWRITE_PNG_COMPRESSION, 9});
				}
				else if (save_as_jpg)
				{
					cv::imwrite(ss.str() + ".jpg", mat, {CV_IMWRITE_JPEG_OPTIMIZE, 1, CV_IMWRITE_JPEG_QUALITY, jpg_quality});
				}

				frame_number ++;
				number_of_processed_frames ++;
				work_completed += 1.0;
				setProgress(work_completed / work_to_be_done);
			}
		}
	}
	catch (const std::exception & e)
	{
		error_shown = true;
		AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "DarkMark - Error!", "An error was detected while processing the video file \"" + current_filename + "\":\n\n" + e.what());
	}
	catch (...)
	{
		error_shown = true;
		AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "DarkMark - Error!", "An unknown error was encountered while processing the video file \"" + current_filename + "\".");
	}

	File dir(base_directory);
	dir.revealToUser();

	if (error_shown == false and threadShouldExit() == false)
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::InfoIcon, "DarkMark", "Extracted " + std::to_string(number_of_processed_frames) + " video frames.");
	}

	return;
}