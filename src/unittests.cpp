#include "unittests.h"

Unit_tests::Unit_tests()
{

}

void Unit_tests::run_all_tests()
{
    qDebug() << "INFO: run_all_tests started.";

    // Useful allocation tests, unaligned, alignedas, etc.
    bool malloc_tests_result = malloc_tests();

    // Testing cmft
    bool cmft_test_result = cmft_image_load_test();

    // Testing WebKit on another process
    bool webkit_test_result = webkit_on_separate_process_test();


    if (!malloc_tests_result                 
            || !cmft_test_result
            || !webkit_test_result
            )
    {
        qDebug() << "ERROR: run_all_tests, a unit test failed.";
    }
    else
    {
        qDebug()  <<"INFO: run_all_tests ended.";
    }
}

bool Unit_tests::malloc_tests()
{
    float* unalignedTest = new float(1.0f);
    float* aligned16test = (float*)aligned_malloc(sizeof(float),16);
    *aligned16test = 16.0f;
    float* aligned32test = (float*)aligned_malloc(sizeof(float),32);
    *aligned32test = 32.0f;
    float* aligned64test = (float*)aligned_malloc(sizeof(float),64);
    *aligned64test = 64.0f;
    float* aligned128test = (float*)aligned_malloc(sizeof(float),128);
    *aligned128test = 128.0f;
    float* aligned256test = (float*)aligned_malloc(sizeof(float),256);
    *aligned256test = 256.0f;

    const bool unalignedResult = unalignedTest != nullptr && *unalignedTest == 1.0f;
    const bool aligned16Result = is_aligned(aligned16test,16) && *aligned16test == 16.0f;
    const bool aligned32Result = is_aligned(aligned16test,16) && *aligned32test == 32.0f;
    const bool aligned64Result = is_aligned(aligned16test,16) && *aligned64test == 64.0f;
    const bool aligned128Result = is_aligned(aligned16test,16) && *aligned128test == 128.0f;
    const bool aligned256Result = is_aligned(aligned16test,16) && *aligned256test == 256.0f;

    bool return_result = false;

    if (    !unalignedResult
        ||  !aligned16Result
        ||  !aligned32Result
        ||  !aligned64Result
        ||  !aligned128Result
        ||  !aligned256Result)
    {
        const size_t bufferSize = 2048;
        const std::string trueString("TRUE");
        const std::string falseString("FALSE");

        std::vector<char> outputBuffer(bufferSize);

#if defined(WIN32)
        int32_t charsWritten = _snprintf(outputBuffer.data()
                                             , bufferSize
                                             , "Unaligned: %f %p %s\n16: %f %p %s\n32: %f %p %s\n64: %f %p %s\n128: %f %p %s\n256: %f %p %s\n"
                                             ,*unalignedTest,(void*)unalignedTest, unalignedResult ? trueString.c_str() : falseString.c_str()
                                             ,*aligned16test,(void*)aligned16test, is_aligned(aligned16test,16) ? trueString.c_str() : falseString.c_str()
                                             ,*aligned32test,(void*)aligned32test, is_aligned(aligned32test,32) ? trueString.c_str() : falseString.c_str()
                                             ,*aligned64test,(void*)aligned64test, is_aligned(aligned64test,64) ? trueString.c_str() : falseString.c_str()
                                             ,*aligned128test,(void*)aligned128test, is_aligned(aligned128test,128) ? trueString.c_str() : falseString.c_str()
                                             ,*aligned256test,(void*)aligned256test, is_aligned(aligned256test,256) ? trueString.c_str() : falseString.c_str()
                                             );
#else
        int32_t charsWritten = std::snprintf(outputBuffer.data()
                                             , bufferSize
                                             , "Unaligned: %f %p %s\n16: %f %p %s\n32: %f %p %s\n64: %f %p %s\n128: %f %p %s\n256: %f %p %s\n"
                                             ,*unalignedTest,(void*)unalignedTest, unalignedResult ? trueString.c_str() : falseString.c_str()
                                             ,*aligned16test,(void*)aligned16test, is_aligned(aligned16test,16) ? trueString.c_str() : falseString.c_str()
                                             ,*aligned32test,(void*)aligned32test, is_aligned(aligned32test,32) ? trueString.c_str() : falseString.c_str()
                                             ,*aligned64test,(void*)aligned64test, is_aligned(aligned64test,64) ? trueString.c_str() : falseString.c_str()
                                             ,*aligned128test,(void*)aligned128test, is_aligned(aligned128test,128) ? trueString.c_str() : falseString.c_str()
                                             ,*aligned256test,(void*)aligned256test, is_aligned(aligned256test,256) ? trueString.c_str() : falseString.c_str()
                                             );
#endif
        if (charsWritten > -1 && charsWritten <= 2048)
        {
            qDebug() << outputBuffer.data();
        }
        else
        {
            qDebug()  << "ERROR: Encoding error in AlignedMallocTest std::snprintf, bufferSize may be too small to fit the formatted string.";
        }
    }
    else
    {
        qDebug() << "SUCCESS: malloc_tests passed.";
        return_result = true;
    }
    delete unalignedTest;
    aligned_free(aligned16test);
    aligned_free(aligned32test);
    aligned_free(aligned64test);
    aligned_free(aligned128test);
    aligned_free(aligned256test);

    return return_result;
}

bool Unit_tests::is_aligned(const void * pointer, size_t byte_count)
{
    if (byte_count == 0) {
        return true;
    }
    else {
        return (((const uintptr_t)pointer % byte_count) == 0);
    }
}

bool Unit_tests::cmft_image_load_test()
{
    FilteredCubemapManager manager;
    bool result = manager.TestLoadImage();

    if (result == true)
    {
        qDebug()  << "SUCCESS: cmft_image_load_test passed.";
        return true;
    }
    else
    {
        qDebug()  << "ERROR: cmft_image_load_test failed.";
        return false;
    }
}

bool Unit_tests::webkit_on_separate_process_test()
{
    /*class  OffScreenWebSurface_event
    {
    public:
        OffScreenWebSurface_event()
            : m_URL(nullptr),
            m_mouse_pos_x(0),
            m_mouse_pos_y(0),
            m_mouse_press_event(false),
            m_mouse_press_event_data(0),
            m_mouse_move_event(false),
            m_mouse_move_event_data(0),
            m_mouse_release_event(false),
            m_mouse_release_event_data(0),
            m_key_press_event(false),
            m_key_press_event_data(0),
            m_key_release_event(false),
            m_key_release_event_data(0),
            m_wheel_event(false),
            m_wheel_event_data(0),
            m_drag_enter_event(false),
            m_drag_enter_event_data(0),
            m_drag_move_event(false),
            m_drag_move_event_data(0),
            m_drag_leave_event(false),
            m_drag_leave_event_data(0),
            m_drop_event(false),
            m_drop_event_data(0),
            m_misc_data(0),
            m_keep_alive(0)
        {
        }
        char* m_URL;
        uint m_mouse_pos_x;
        uint m_mouse_pos_y;
        bool m_mouse_press_event;
        uintptr_t m_mouse_press_event_data;
        bool m_mouse_move_event;
        uintptr_t m_mouse_move_event_data;
        bool m_mouse_release_event;
        uintptr_t m_mouse_release_event_data;
        bool m_key_press_event;
        uintptr_t m_key_press_event_data;
        bool m_key_release_event;
        uintptr_t m_key_release_event_data;
        bool m_wheel_event;
        uintptr_t m_wheel_event_data;
        bool m_drag_enter_event;
        uintptr_t m_drag_enter_event_data;
        bool m_drag_move_event;
        uintptr_t m_drag_move_event_data;
        bool m_drag_leave_event;
        uintptr_t m_drag_leave_event_data;
        bool m_drop_event;
        uintptr_t m_drop_event_data;
        uintptr_t m_misc_data;
        uintptr_t m_keep_alive;
    };

	QSharedMemory m_shared_memory;
	QSharedMemory m_shared_memory_image;
	QProcess* m_webkit_process = new QProcess();
	QString key = QString(QString::number((quintptr)m_webkit_process));
	m_shared_memory.setKey(key);
	if (m_shared_memory.isAttached())
	{
		m_shared_memory.detach();
	}

	if (!m_shared_memory.create(sizeof(OffScreenWebSurface_event)))
	{
		qDebug() << "ERROR: webkit_on_separate_process_test Failed to allocate shared memory...";
		delete m_webkit_process;
		return false;
	}

	// Create Secondary shared_memory to store the images in
	m_shared_memory_image.setKey(key + "_image");
	if (m_shared_memory_image.isAttached())
	{
		m_shared_memory_image.detach();
	}

	if (!m_shared_memory_image.create(1280 * 720 * 4)) // Format_RGBA8888_Premultiplied
	{
		qDebug() << "ERROR: webkit_on_separate_process_test Failed to allocate shared memory...";
		delete m_webkit_process;
		return false;
	}

	QString application_path = QCoreApplication::applicationDirPath();

	QString webkit_process_location = application_path + "/JanusVR_QtWebKit.exe";
	m_webkit_process->setProgram(webkit_process_location);
	// Use the Process pointer as the key for the QSharedMemory, this is passed as the only argument
	QStringList args;
	args.append(QString("-key"));
	args.append(QStringList(key));
	m_webkit_process->setArguments(args);
	m_webkit_process->setProcessChannelMode(QProcess::ForwardedChannels);
	m_webkit_process->start();
	m_webkit_process->waitForStarted();

	OffScreenWebSurface_event* shared_memory_data = (OffScreenWebSurface_event*)m_shared_memory.data();
	
	QImage recovered_image(1280, 720, QImage::Format_RGBA8888_Premultiplied);

	if (m_webkit_process->waitForFinished(5000) == false)
	{
		m_webkit_process->kill();
	}
	
	// Copy the serialized QImage into the shared memory
	// I feel bad casting off the const but the alternative of QImage::bits() 
	// does a deep copy of the data which is not what I'm trying to do here
	char* to = (char*)recovered_image.constBits();
	const char* from = (char*)m_shared_memory_image.data();
	memcpy(to, from, m_shared_memory_image.size());

	// Save out the recovered image for visual inspection
	recovered_image.save(application_path + "/shared_memory_test_image_parent.jpg", "jpg", 95);

    qDebug() << "SUCCESS: webkit_on_separate_process_test passed.";
	delete m_webkit_process;*/
	return true;
}

