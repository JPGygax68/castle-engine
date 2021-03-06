/*
 Copyright 2013-2017 Jan Adamec, Michalis Kamburelis.
 
 This file is part of "Castle Game Engine".
 
 "Castle Game Engine" is free software; see the file COPYING.txt,
 included in this distribution, for details about the copyright.
 
 "Castle Game Engine" is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 
 ----------------------------------------------------------------------------
 */

#import <Foundation/Foundation.h>

@protocol FileOpenCtlDelegate
- (void)fileSelectedToOpen:(NSString*)selectedFile;
@end

@interface FileOpenController : UITableViewController

@property (nonatomic, weak) id<FileOpenCtlDelegate> delegate;

@end
