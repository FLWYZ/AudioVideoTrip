//
//  ViewController.swift
//  AudioVideoTrip
//
//  Created by fanglei on 2020/11/19.
//

import Cocoa

class ViewController: NSViewController {

    private var isRecording = false
    
    private var thread: Thread?
    
    override func viewDidLoad() {
        super.viewDidLoad()

        view.setFrameSize(.init(width: 320, height: 240))
        button.frame.origin = .init(x: 0, y: (240 - button.frame.height)/2.0)
        view.addSubview(button)
    }

    @objc
    private func onClickButton() {
        if isRecording { // 停止
            stop_record()
        } else { // 开始录制
            thread = Thread(target: self, selector: #selector(record), object: nil)
            thread?.start()
        }
        isRecording.toggle()
        button.title = isRecording ? "停止录音" : "开始录音"
    }
    
    @objc
    private func record() {
        let string = "/Users/fanglei/Desktop/simple/out.yuv".cString(using: .utf8)
        record_video(UnsafeMutablePointer<Int8>(mutating: string))
    }
    
    private lazy var button: NSButton = {
        let btn = NSButton(title: "开始录音", target: self, action: #selector(onClickButton))
        btn.bezelStyle = .rounded
        btn.frame.size = .init(width: 100, height: 30)
        return btn
    }()
}

