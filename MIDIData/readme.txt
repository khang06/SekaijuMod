/******************************************************************************/
/*                                                                            */
/*  MIDIData���C�u�����u�͂��߂ɂ��ǂ݂��������v           (C)2002-2020 ����  */
/*                                                                            */
/******************************************************************************/

���̂��т́AMIDIData���C�u�������_�E�����[�h���Ă��������A���͂��󂯎��ɂ�
���Ă��������A���ɂ��肪�Ƃ��������܂��B

MIDIData���C�u�����x�́A�t���[�ŃI�[�v���\�[�X�́AMIDI�f�[�^�쐬�E�ҏW�p���C�u
�����ł��B���̃��C�u�����́AMIDI�f�[�^�̍쐬�A���E���V�[�P���X�t�@�C��(*.skj)��
�ݍ��݁E�ۑ��A��Cakewalk�V�[�P���X�t�@�C��(*.wrk)�ǂݍ��݁ACherry�V�[�P���X�t�@
�C��(*.chy)�ǂݍ��݁E�ۑ��A�X�^���_�[�hMIDI�t�@�C��(*.mid)�ǂݍ��݁E�ۑ��AMIDI
CSV�t�@�C��(*.csv)�ǂݍ��݁E�ۑ��AMIDI�g���b�N�̍쐬�E�ǉ��E�ύX�E�폜�AMIDI�C�x
���g(���^�C�x���g�E�V�X�e���G�N�X�N���[�V�u�C�x���g���܂�)�̍쐬�E�ǉ��E�ύX�E��
���ȂǁAMIDI�f�[�^�̂�����v�f�ɐv���ɃA�N�Z�X����֐���񋟂��Ă��܂��B

�^�C���x�[�X�́ATPQN�x�[�X(����\=1�`32767)��SMPTE�x�[�X(24,25,29.97,30�t���[��
�^�b)(����\=1�`255)�̗����ɑΉ��B�^�C���x�[�X�擾�E�ϊ��֐��������B�t�H�[�}�b�g
��0/1/2�ɑΉ��B�t�H�[�}�b�g�擾�E�ϊ��֐��������B�܂��AMIDI�f�[�^�̃^�C�g���E��
�쌠�E�R�����g�E�J�n�^�C���E�I���^�C���Ȃǂ��ȈՂɎ擾�E�ݒ肷��֐����p�ӂ��܂�
���B

�g���b�N�̃e���|�f�[�^����͂��āA�^�C���R�[�h���~���b�ɕϊ�������A���q�L��
����͂��āA�^�C���R�[�h�����߁F���F�e�B�b�N�ɕϊ�����֐����p�ӂ��Ă��܂��B
�������Z�͕K�v�ɉ�����64�r�b�g�Ƃ��A�e���|�`�F���W�̌������Ȃł��v�Z�덷��1�~��
�b�ȓ��ɗ}���Ă��܂��B


�y�Y�t�t�@�C���z

MIDIData
��readme.txt        �͂��߂ɂ��ǂ݂�������(���{��)
��readme_en.txt     �͂��߂ɂ��ǂ݂�������(�p��)
��license.txt       ���C�Z���X(�����E�p��)
��MIDIData.c        C�\�[�X�t�@�C��
��MIDIData.h        C/C++�p�w�b�_�[�t�@�C��
��MIDIData.def      C/C++�p���W���[����`�t�@�C��(dll�����Ƃ��ɕK�v)
��MIDIData.mak      C/C++�p���C�N�t�@�C��
��MIDIData.sln      Visual C++ 2008 Service Pack 1�p�\�����[�V�����t�@�C��
��MIDIData.vcproj   Visual C++ 2008 Service Pack 1�p�v���W�F�N�g���[�N�X�y�[�X
��MIDIData.bas      Visual Basic 4.0/5.0/6.0�p�C���|�[�g���W���[��
��Debug
����MIDIDatad.lib   �I�u�W�F�N�g���C�u�������W���[��(�f�o�b�O�p)
����MIDIDatad.dll   �_�C�i�~�b�N�����N���C�u����(�f�o�b�O�p)
��Release
����MIDIData.lib    �I�u�W�F�N�g���C�u�������W���[��(�����[�X�p)
����MIDIData.dll    �_�C�i�~�b�N�����N���C�u����(�����[�X�p)
��docs
�@��MIDIData.html   �����K�C�h�u�b�N�{��(���{��)
�@��MIDIData_en.html �����K�C�h�u�b�N�{��(�p��)
�@��MIDIDR01.gif    �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIDR02.gif    �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIDR03.gif    �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIDR04.gif    �����K�C�h�u�b�N�Ŏg���Ă���}


�y�g�p���@�z

�E���ׂẴt�@�C�����𓀂��Ă��������B
�EMIDIData.h��VisualC++���C���X�g�[�������t�H���_����include�t�H���_���ɃR�s�[���Ă��������B
�EMIDIData.lib��VisualC++���C���X�g�[�������t�H���_����lib�t�H���_���ɃR�s�[���Ă��������B
�EMIDIData.dll�t�@�C����c:\windows\System32\��(32bitOS�̏ꍇ)�A
�@c:\windows\SysWOW64\��(64bitOS�̏ꍇ)�ɃR�s�[���Ă��������B
�EMIDIData���C�u�������g�p����\�[�X�ł́A#include "MIDIData.h"���s���Ă��������B
�EMIDIData���C�u�������g�p����v���W�F�N�g�ł́AVisualC++�����g�p�̏ꍇ�A
�E�u�r���h(B)�v-�u(�v���W�F�N�g��)�̃v���p�e�B�v����u(�v���W�F�N�g��)�̃v���p�e�B�y�[�W�v�Ƃ���
�@�_�C�A���O��\�������A���̒��́u�\���ƃv���p�e�B�v�́u�����J�v�́u���́v�̒��ɁA
�@�u�ǉ��̈ˑ��t�@�C���v�Ƃ������ڂ�����̂ŁA������MIDIData.lib��ǉ����Ă��������B
�E�A�v���P�[�V������z�z����ۂɂ́AMIDIData.dll��exe�t�@�C���Ɠ����t�H���_�ɓY�t���Ă��������B

�y���C�Z���X�z

�E���̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��Ĕz�z����܂��B
�E���̃��C�u�����͑S���̖��ۏ؂ł��B
�E���̃��C�u�������g�p�������ʐ��������Q�ɂ��܂��č�҂͈�ؐӔC�𕉂��܂���B
�@���炩���߂��������������B
���ڂ����́ALGPL�̓��{���(http://www.opensource.gr.jp/lesser/lgpl.ja.html)�����Q�Ƃ��������B

�y�A����z

�E���[���A�h���X(��) ee65051@yahoo.co.jp
�E�v���W�F�N�g�E�F�u�T�C�g https://openmidiproject.osdn.jp/index.html

