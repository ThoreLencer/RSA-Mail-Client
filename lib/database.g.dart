// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'database.dart';

// **************************************************************************
// MoorGenerator
// **************************************************************************

// ignore_for_file: unnecessary_brace_in_string_interps, unnecessary_this
class Message extends DataClass implements Insertable<Message> {
  final int ID;
  final int MessageID;
  final int UserID;
  final int Sender;
  final int Receiver;
  final String MessageStr;
  final int Read;
  final String Date;
  final String ReceiveDate;
  Message(
      {required this.ID,
      required this.MessageID,
      required this.UserID,
      required this.Sender,
      required this.Receiver,
      required this.MessageStr,
      required this.Read,
      required this.Date,
      required this.ReceiveDate});
  factory Message.fromData(Map<String, dynamic> data, {String? prefix}) {
    final effectivePrefix = prefix ?? '';
    return Message(
      ID: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}id'])!,
      MessageID: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}message_i_d'])!,
      UserID: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}user_i_d'])!,
      Sender: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}sender'])!,
      Receiver: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}receiver'])!,
      MessageStr: const StringType()
          .mapFromDatabaseResponse(data['${effectivePrefix}message_str'])!,
      Read: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}read'])!,
      Date: const StringType()
          .mapFromDatabaseResponse(data['${effectivePrefix}date'])!,
      ReceiveDate: const StringType()
          .mapFromDatabaseResponse(data['${effectivePrefix}receive_date'])!,
    );
  }
  @override
  Map<String, Expression> toColumns(bool nullToAbsent) {
    final map = <String, Expression>{};
    map['id'] = Variable<int>(ID);
    map['message_i_d'] = Variable<int>(MessageID);
    map['user_i_d'] = Variable<int>(UserID);
    map['sender'] = Variable<int>(Sender);
    map['receiver'] = Variable<int>(Receiver);
    map['message_str'] = Variable<String>(MessageStr);
    map['read'] = Variable<int>(Read);
    map['date'] = Variable<String>(Date);
    map['receive_date'] = Variable<String>(ReceiveDate);
    return map;
  }

  MessagesCompanion toCompanion(bool nullToAbsent) {
    return MessagesCompanion(
      ID: Value(ID),
      MessageID: Value(MessageID),
      UserID: Value(UserID),
      Sender: Value(Sender),
      Receiver: Value(Receiver),
      MessageStr: Value(MessageStr),
      Read: Value(Read),
      Date: Value(Date),
      ReceiveDate: Value(ReceiveDate),
    );
  }

  factory Message.fromJson(Map<String, dynamic> json,
      {ValueSerializer? serializer}) {
    serializer ??= driftRuntimeOptions.defaultSerializer;
    return Message(
      ID: serializer.fromJson<int>(json['ID']),
      MessageID: serializer.fromJson<int>(json['MessageID']),
      UserID: serializer.fromJson<int>(json['UserID']),
      Sender: serializer.fromJson<int>(json['Sender']),
      Receiver: serializer.fromJson<int>(json['Receiver']),
      MessageStr: serializer.fromJson<String>(json['MessageStr']),
      Read: serializer.fromJson<int>(json['Read']),
      Date: serializer.fromJson<String>(json['Date']),
      ReceiveDate: serializer.fromJson<String>(json['ReceiveDate']),
    );
  }
  @override
  Map<String, dynamic> toJson({ValueSerializer? serializer}) {
    serializer ??= driftRuntimeOptions.defaultSerializer;
    return <String, dynamic>{
      'ID': serializer.toJson<int>(ID),
      'MessageID': serializer.toJson<int>(MessageID),
      'UserID': serializer.toJson<int>(UserID),
      'Sender': serializer.toJson<int>(Sender),
      'Receiver': serializer.toJson<int>(Receiver),
      'MessageStr': serializer.toJson<String>(MessageStr),
      'Read': serializer.toJson<int>(Read),
      'Date': serializer.toJson<String>(Date),
      'ReceiveDate': serializer.toJson<String>(ReceiveDate),
    };
  }

  Message copyWith(
          {int? ID,
          int? MessageID,
          int? UserID,
          int? Sender,
          int? Receiver,
          String? MessageStr,
          int? Read,
          String? Date,
          String? ReceiveDate}) =>
      Message(
        ID: ID ?? this.ID,
        MessageID: MessageID ?? this.MessageID,
        UserID: UserID ?? this.UserID,
        Sender: Sender ?? this.Sender,
        Receiver: Receiver ?? this.Receiver,
        MessageStr: MessageStr ?? this.MessageStr,
        Read: Read ?? this.Read,
        Date: Date ?? this.Date,
        ReceiveDate: ReceiveDate ?? this.ReceiveDate,
      );
  @override
  String toString() {
    return (StringBuffer('Message(')
          ..write('ID: $ID, ')
          ..write('MessageID: $MessageID, ')
          ..write('UserID: $UserID, ')
          ..write('Sender: $Sender, ')
          ..write('Receiver: $Receiver, ')
          ..write('MessageStr: $MessageStr, ')
          ..write('Read: $Read, ')
          ..write('Date: $Date, ')
          ..write('ReceiveDate: $ReceiveDate')
          ..write(')'))
        .toString();
  }

  @override
  int get hashCode => Object.hash(ID, MessageID, UserID, Sender, Receiver,
      MessageStr, Read, Date, ReceiveDate);
  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      (other is Message &&
          other.ID == this.ID &&
          other.MessageID == this.MessageID &&
          other.UserID == this.UserID &&
          other.Sender == this.Sender &&
          other.Receiver == this.Receiver &&
          other.MessageStr == this.MessageStr &&
          other.Read == this.Read &&
          other.Date == this.Date &&
          other.ReceiveDate == this.ReceiveDate);
}

class MessagesCompanion extends UpdateCompanion<Message> {
  final Value<int> ID;
  final Value<int> MessageID;
  final Value<int> UserID;
  final Value<int> Sender;
  final Value<int> Receiver;
  final Value<String> MessageStr;
  final Value<int> Read;
  final Value<String> Date;
  final Value<String> ReceiveDate;
  const MessagesCompanion({
    this.ID = const Value.absent(),
    this.MessageID = const Value.absent(),
    this.UserID = const Value.absent(),
    this.Sender = const Value.absent(),
    this.Receiver = const Value.absent(),
    this.MessageStr = const Value.absent(),
    this.Read = const Value.absent(),
    this.Date = const Value.absent(),
    this.ReceiveDate = const Value.absent(),
  });
  MessagesCompanion.insert({
    this.ID = const Value.absent(),
    required int MessageID,
    required int UserID,
    required int Sender,
    required int Receiver,
    required String MessageStr,
    required int Read,
    required String Date,
    required String ReceiveDate,
  })  : MessageID = Value(MessageID),
        UserID = Value(UserID),
        Sender = Value(Sender),
        Receiver = Value(Receiver),
        MessageStr = Value(MessageStr),
        Read = Value(Read),
        Date = Value(Date),
        ReceiveDate = Value(ReceiveDate);
  static Insertable<Message> custom({
    Expression<int>? ID,
    Expression<int>? MessageID,
    Expression<int>? UserID,
    Expression<int>? Sender,
    Expression<int>? Receiver,
    Expression<String>? MessageStr,
    Expression<int>? Read,
    Expression<String>? Date,
    Expression<String>? ReceiveDate,
  }) {
    return RawValuesInsertable({
      if (ID != null) 'id': ID,
      if (MessageID != null) 'message_i_d': MessageID,
      if (UserID != null) 'user_i_d': UserID,
      if (Sender != null) 'sender': Sender,
      if (Receiver != null) 'receiver': Receiver,
      if (MessageStr != null) 'message_str': MessageStr,
      if (Read != null) 'read': Read,
      if (Date != null) 'date': Date,
      if (ReceiveDate != null) 'receive_date': ReceiveDate,
    });
  }

  MessagesCompanion copyWith(
      {Value<int>? ID,
      Value<int>? MessageID,
      Value<int>? UserID,
      Value<int>? Sender,
      Value<int>? Receiver,
      Value<String>? MessageStr,
      Value<int>? Read,
      Value<String>? Date,
      Value<String>? ReceiveDate}) {
    return MessagesCompanion(
      ID: ID ?? this.ID,
      MessageID: MessageID ?? this.MessageID,
      UserID: UserID ?? this.UserID,
      Sender: Sender ?? this.Sender,
      Receiver: Receiver ?? this.Receiver,
      MessageStr: MessageStr ?? this.MessageStr,
      Read: Read ?? this.Read,
      Date: Date ?? this.Date,
      ReceiveDate: ReceiveDate ?? this.ReceiveDate,
    );
  }

  @override
  Map<String, Expression> toColumns(bool nullToAbsent) {
    final map = <String, Expression>{};
    if (ID.present) {
      map['id'] = Variable<int>(ID.value);
    }
    if (MessageID.present) {
      map['message_i_d'] = Variable<int>(MessageID.value);
    }
    if (UserID.present) {
      map['user_i_d'] = Variable<int>(UserID.value);
    }
    if (Sender.present) {
      map['sender'] = Variable<int>(Sender.value);
    }
    if (Receiver.present) {
      map['receiver'] = Variable<int>(Receiver.value);
    }
    if (MessageStr.present) {
      map['message_str'] = Variable<String>(MessageStr.value);
    }
    if (Read.present) {
      map['read'] = Variable<int>(Read.value);
    }
    if (Date.present) {
      map['date'] = Variable<String>(Date.value);
    }
    if (ReceiveDate.present) {
      map['receive_date'] = Variable<String>(ReceiveDate.value);
    }
    return map;
  }

  @override
  String toString() {
    return (StringBuffer('MessagesCompanion(')
          ..write('ID: $ID, ')
          ..write('MessageID: $MessageID, ')
          ..write('UserID: $UserID, ')
          ..write('Sender: $Sender, ')
          ..write('Receiver: $Receiver, ')
          ..write('MessageStr: $MessageStr, ')
          ..write('Read: $Read, ')
          ..write('Date: $Date, ')
          ..write('ReceiveDate: $ReceiveDate')
          ..write(')'))
        .toString();
  }
}

class $MessagesTable extends Messages with TableInfo<$MessagesTable, Message> {
  final GeneratedDatabase _db;
  final String? _alias;
  $MessagesTable(this._db, [this._alias]);
  final VerificationMeta _IDMeta = const VerificationMeta('ID');
  @override
  late final GeneratedColumn<int?> ID = GeneratedColumn<int?>(
      'id', aliasedName, false,
      type: const IntType(),
      requiredDuringInsert: false,
      defaultConstraints: 'PRIMARY KEY AUTOINCREMENT');
  final VerificationMeta _MessageIDMeta = const VerificationMeta('MessageID');
  @override
  late final GeneratedColumn<int?> MessageID = GeneratedColumn<int?>(
      'message_i_d', aliasedName, false,
      type: const IntType(), requiredDuringInsert: true);
  final VerificationMeta _UserIDMeta = const VerificationMeta('UserID');
  @override
  late final GeneratedColumn<int?> UserID = GeneratedColumn<int?>(
      'user_i_d', aliasedName, false,
      type: const IntType(), requiredDuringInsert: true);
  final VerificationMeta _SenderMeta = const VerificationMeta('Sender');
  @override
  late final GeneratedColumn<int?> Sender = GeneratedColumn<int?>(
      'sender', aliasedName, false,
      type: const IntType(), requiredDuringInsert: true);
  final VerificationMeta _ReceiverMeta = const VerificationMeta('Receiver');
  @override
  late final GeneratedColumn<int?> Receiver = GeneratedColumn<int?>(
      'receiver', aliasedName, false,
      type: const IntType(), requiredDuringInsert: true);
  final VerificationMeta _MessageStrMeta = const VerificationMeta('MessageStr');
  @override
  late final GeneratedColumn<String?> MessageStr = GeneratedColumn<String?>(
      'message_str', aliasedName, false,
      type: const StringType(), requiredDuringInsert: true);
  final VerificationMeta _ReadMeta = const VerificationMeta('Read');
  @override
  late final GeneratedColumn<int?> Read = GeneratedColumn<int?>(
      'read', aliasedName, false,
      type: const IntType(), requiredDuringInsert: true);
  final VerificationMeta _DateMeta = const VerificationMeta('Date');
  @override
  late final GeneratedColumn<String?> Date = GeneratedColumn<String?>(
      'date', aliasedName, false,
      type: const StringType(), requiredDuringInsert: true);
  final VerificationMeta _ReceiveDateMeta =
      const VerificationMeta('ReceiveDate');
  @override
  late final GeneratedColumn<String?> ReceiveDate = GeneratedColumn<String?>(
      'receive_date', aliasedName, false,
      type: const StringType(), requiredDuringInsert: true);
  @override
  List<GeneratedColumn> get $columns => [
        ID,
        MessageID,
        UserID,
        Sender,
        Receiver,
        MessageStr,
        Read,
        Date,
        ReceiveDate
      ];
  @override
  String get aliasedName => _alias ?? 'messages';
  @override
  String get actualTableName => 'messages';
  @override
  VerificationContext validateIntegrity(Insertable<Message> instance,
      {bool isInserting = false}) {
    final context = VerificationContext();
    final data = instance.toColumns(true);
    if (data.containsKey('id')) {
      context.handle(_IDMeta, ID.isAcceptableOrUnknown(data['id']!, _IDMeta));
    }
    if (data.containsKey('message_i_d')) {
      context.handle(
          _MessageIDMeta,
          MessageID.isAcceptableOrUnknown(
              data['message_i_d']!, _MessageIDMeta));
    } else if (isInserting) {
      context.missing(_MessageIDMeta);
    }
    if (data.containsKey('user_i_d')) {
      context.handle(_UserIDMeta,
          UserID.isAcceptableOrUnknown(data['user_i_d']!, _UserIDMeta));
    } else if (isInserting) {
      context.missing(_UserIDMeta);
    }
    if (data.containsKey('sender')) {
      context.handle(_SenderMeta,
          Sender.isAcceptableOrUnknown(data['sender']!, _SenderMeta));
    } else if (isInserting) {
      context.missing(_SenderMeta);
    }
    if (data.containsKey('receiver')) {
      context.handle(_ReceiverMeta,
          Receiver.isAcceptableOrUnknown(data['receiver']!, _ReceiverMeta));
    } else if (isInserting) {
      context.missing(_ReceiverMeta);
    }
    if (data.containsKey('message_str')) {
      context.handle(
          _MessageStrMeta,
          MessageStr.isAcceptableOrUnknown(
              data['message_str']!, _MessageStrMeta));
    } else if (isInserting) {
      context.missing(_MessageStrMeta);
    }
    if (data.containsKey('read')) {
      context.handle(
          _ReadMeta, Read.isAcceptableOrUnknown(data['read']!, _ReadMeta));
    } else if (isInserting) {
      context.missing(_ReadMeta);
    }
    if (data.containsKey('date')) {
      context.handle(
          _DateMeta, Date.isAcceptableOrUnknown(data['date']!, _DateMeta));
    } else if (isInserting) {
      context.missing(_DateMeta);
    }
    if (data.containsKey('receive_date')) {
      context.handle(
          _ReceiveDateMeta,
          ReceiveDate.isAcceptableOrUnknown(
              data['receive_date']!, _ReceiveDateMeta));
    } else if (isInserting) {
      context.missing(_ReceiveDateMeta);
    }
    return context;
  }

  @override
  Set<GeneratedColumn> get $primaryKey => {ID};
  @override
  Message map(Map<String, dynamic> data, {String? tablePrefix}) {
    return Message.fromData(data,
        prefix: tablePrefix != null ? '$tablePrefix.' : null);
  }

  @override
  $MessagesTable createAlias(String alias) {
    return $MessagesTable(_db, alias);
  }
}

class Contact extends DataClass implements Insertable<Contact> {
  final int ID;
  final int UserID;
  final int ContactID;
  final String Name;
  Contact(
      {required this.ID,
      required this.UserID,
      required this.ContactID,
      required this.Name});
  factory Contact.fromData(Map<String, dynamic> data, {String? prefix}) {
    final effectivePrefix = prefix ?? '';
    return Contact(
      ID: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}id'])!,
      UserID: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}user_i_d'])!,
      ContactID: const IntType()
          .mapFromDatabaseResponse(data['${effectivePrefix}contact_i_d'])!,
      Name: const StringType()
          .mapFromDatabaseResponse(data['${effectivePrefix}name'])!,
    );
  }
  @override
  Map<String, Expression> toColumns(bool nullToAbsent) {
    final map = <String, Expression>{};
    map['id'] = Variable<int>(ID);
    map['user_i_d'] = Variable<int>(UserID);
    map['contact_i_d'] = Variable<int>(ContactID);
    map['name'] = Variable<String>(Name);
    return map;
  }

  ContactsCompanion toCompanion(bool nullToAbsent) {
    return ContactsCompanion(
      ID: Value(ID),
      UserID: Value(UserID),
      ContactID: Value(ContactID),
      Name: Value(Name),
    );
  }

  factory Contact.fromJson(Map<String, dynamic> json,
      {ValueSerializer? serializer}) {
    serializer ??= driftRuntimeOptions.defaultSerializer;
    return Contact(
      ID: serializer.fromJson<int>(json['ID']),
      UserID: serializer.fromJson<int>(json['UserID']),
      ContactID: serializer.fromJson<int>(json['ContactID']),
      Name: serializer.fromJson<String>(json['Name']),
    );
  }
  @override
  Map<String, dynamic> toJson({ValueSerializer? serializer}) {
    serializer ??= driftRuntimeOptions.defaultSerializer;
    return <String, dynamic>{
      'ID': serializer.toJson<int>(ID),
      'UserID': serializer.toJson<int>(UserID),
      'ContactID': serializer.toJson<int>(ContactID),
      'Name': serializer.toJson<String>(Name),
    };
  }

  Contact copyWith({int? ID, int? UserID, int? ContactID, String? Name}) =>
      Contact(
        ID: ID ?? this.ID,
        UserID: UserID ?? this.UserID,
        ContactID: ContactID ?? this.ContactID,
        Name: Name ?? this.Name,
      );
  @override
  String toString() {
    return (StringBuffer('Contact(')
          ..write('ID: $ID, ')
          ..write('UserID: $UserID, ')
          ..write('ContactID: $ContactID, ')
          ..write('Name: $Name')
          ..write(')'))
        .toString();
  }

  @override
  int get hashCode => Object.hash(ID, UserID, ContactID, Name);
  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      (other is Contact &&
          other.ID == this.ID &&
          other.UserID == this.UserID &&
          other.ContactID == this.ContactID &&
          other.Name == this.Name);
}

class ContactsCompanion extends UpdateCompanion<Contact> {
  final Value<int> ID;
  final Value<int> UserID;
  final Value<int> ContactID;
  final Value<String> Name;
  const ContactsCompanion({
    this.ID = const Value.absent(),
    this.UserID = const Value.absent(),
    this.ContactID = const Value.absent(),
    this.Name = const Value.absent(),
  });
  ContactsCompanion.insert({
    this.ID = const Value.absent(),
    required int UserID,
    required int ContactID,
    required String Name,
  })  : UserID = Value(UserID),
        ContactID = Value(ContactID),
        Name = Value(Name);
  static Insertable<Contact> custom({
    Expression<int>? ID,
    Expression<int>? UserID,
    Expression<int>? ContactID,
    Expression<String>? Name,
  }) {
    return RawValuesInsertable({
      if (ID != null) 'id': ID,
      if (UserID != null) 'user_i_d': UserID,
      if (ContactID != null) 'contact_i_d': ContactID,
      if (Name != null) 'name': Name,
    });
  }

  ContactsCompanion copyWith(
      {Value<int>? ID,
      Value<int>? UserID,
      Value<int>? ContactID,
      Value<String>? Name}) {
    return ContactsCompanion(
      ID: ID ?? this.ID,
      UserID: UserID ?? this.UserID,
      ContactID: ContactID ?? this.ContactID,
      Name: Name ?? this.Name,
    );
  }

  @override
  Map<String, Expression> toColumns(bool nullToAbsent) {
    final map = <String, Expression>{};
    if (ID.present) {
      map['id'] = Variable<int>(ID.value);
    }
    if (UserID.present) {
      map['user_i_d'] = Variable<int>(UserID.value);
    }
    if (ContactID.present) {
      map['contact_i_d'] = Variable<int>(ContactID.value);
    }
    if (Name.present) {
      map['name'] = Variable<String>(Name.value);
    }
    return map;
  }

  @override
  String toString() {
    return (StringBuffer('ContactsCompanion(')
          ..write('ID: $ID, ')
          ..write('UserID: $UserID, ')
          ..write('ContactID: $ContactID, ')
          ..write('Name: $Name')
          ..write(')'))
        .toString();
  }
}

class $ContactsTable extends Contacts with TableInfo<$ContactsTable, Contact> {
  final GeneratedDatabase _db;
  final String? _alias;
  $ContactsTable(this._db, [this._alias]);
  final VerificationMeta _IDMeta = const VerificationMeta('ID');
  @override
  late final GeneratedColumn<int?> ID = GeneratedColumn<int?>(
      'id', aliasedName, false,
      type: const IntType(),
      requiredDuringInsert: false,
      defaultConstraints: 'PRIMARY KEY AUTOINCREMENT');
  final VerificationMeta _UserIDMeta = const VerificationMeta('UserID');
  @override
  late final GeneratedColumn<int?> UserID = GeneratedColumn<int?>(
      'user_i_d', aliasedName, false,
      type: const IntType(), requiredDuringInsert: true);
  final VerificationMeta _ContactIDMeta = const VerificationMeta('ContactID');
  @override
  late final GeneratedColumn<int?> ContactID = GeneratedColumn<int?>(
      'contact_i_d', aliasedName, false,
      type: const IntType(), requiredDuringInsert: true);
  final VerificationMeta _NameMeta = const VerificationMeta('Name');
  @override
  late final GeneratedColumn<String?> Name = GeneratedColumn<String?>(
      'name', aliasedName, false,
      type: const StringType(), requiredDuringInsert: true);
  @override
  List<GeneratedColumn> get $columns => [ID, UserID, ContactID, Name];
  @override
  String get aliasedName => _alias ?? 'contacts';
  @override
  String get actualTableName => 'contacts';
  @override
  VerificationContext validateIntegrity(Insertable<Contact> instance,
      {bool isInserting = false}) {
    final context = VerificationContext();
    final data = instance.toColumns(true);
    if (data.containsKey('id')) {
      context.handle(_IDMeta, ID.isAcceptableOrUnknown(data['id']!, _IDMeta));
    }
    if (data.containsKey('user_i_d')) {
      context.handle(_UserIDMeta,
          UserID.isAcceptableOrUnknown(data['user_i_d']!, _UserIDMeta));
    } else if (isInserting) {
      context.missing(_UserIDMeta);
    }
    if (data.containsKey('contact_i_d')) {
      context.handle(
          _ContactIDMeta,
          ContactID.isAcceptableOrUnknown(
              data['contact_i_d']!, _ContactIDMeta));
    } else if (isInserting) {
      context.missing(_ContactIDMeta);
    }
    if (data.containsKey('name')) {
      context.handle(
          _NameMeta, Name.isAcceptableOrUnknown(data['name']!, _NameMeta));
    } else if (isInserting) {
      context.missing(_NameMeta);
    }
    return context;
  }

  @override
  Set<GeneratedColumn> get $primaryKey => {ID};
  @override
  Contact map(Map<String, dynamic> data, {String? tablePrefix}) {
    return Contact.fromData(data,
        prefix: tablePrefix != null ? '$tablePrefix.' : null);
  }

  @override
  $ContactsTable createAlias(String alias) {
    return $ContactsTable(_db, alias);
  }
}

abstract class _$MailDatabase extends GeneratedDatabase {
  _$MailDatabase(QueryExecutor e) : super(SqlTypeSystem.defaultInstance, e);
  late final $MessagesTable messages = $MessagesTable(this);
  late final $ContactsTable contacts = $ContactsTable(this);
  @override
  Iterable<TableInfo> get allTables => allSchemaEntities.whereType<TableInfo>();
  @override
  List<DatabaseSchemaEntity> get allSchemaEntities => [messages, contacts];
}
