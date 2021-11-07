/*
    Copyright 2021 Robin Zhang & Labs

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

 */

#include "mac_utilities.h"
#import <Cocoa/Cocoa.h>

MacUtilities::MacUtilities(QObject *parent) : QObject(parent)
{

}

void MacUtilities::showFileInFinder(const QString &filepath)
{
    NSString *strFilepath = [NSString stringWithUTF8String:filepath.toUtf8().constData()];
    NSURL *fileURL = [NSURL fileURLWithPath:strFilepath];
    NSArray *fileURLs = [NSArray arrayWithObjects:fileURL, nil];
    [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];
}

void MacUtilities::setWindowIsModified(QWindow *window, bool isModified)
{
    NSWindow *win = [(NSView *)window->winId() window];
    [win setDocumentEdited:isModified];
}

void MacUtilities::setWindowTitleWithFile(QWindow *window, const QString &filepath)
{
    NSString *strFilepath = [NSString stringWithUTF8String:filepath.toUtf8().constData()];
    NSWindow *win = [(NSView *)window->winId() window];
    [win setRepresentedFilename: strFilepath];
    //[win setRepresentedURL: [NSURL fileURLWithPath:strFilepath]];
}

void MacUtilities::setWindowWithoutTitlebar(QWindow *window)
{
    NSWindow *win = [(NSView *)window->winId() window];
    [win setTitleVisibility: NSWindowTitleHidden];
    [win setTitlebarAppearsTransparent: YES];
    [win setMovableByWindowBackground: YES];

    /*
    for (id subview in win.contentView.superview.subviews) {
        if ([subview isKindOfClass:NSClassFromString(@"NSTitlebarContainerView")]) {
            NSView *titlebarView = [subview subviews][0];
            for (id button in titlebarView.subviews) {
                if ([button isKindOfClass:[NSButton class]]) {
                    [button setHidden:YES];
                }
            }
        }
    }
    */
}
